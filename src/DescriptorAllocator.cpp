#include "DescriptorAllocator.h"

using namespace Plover;

void DescriptorAllocator::init(VkDevice device) {
    this->device = device;
}

void DescriptorAllocator::cleanup() {
	for (auto pool : freePools) {
        vkDestroyDescriptorPool(device, pool, nullptr);
    }
    for (auto pool : usedPools) {
        vkDestroyDescriptorPool(device, pool, nullptr);
    }
}

void DescriptorAllocator::allocate(uint32_t setCount, VkDescriptorSet* sets, VkDescriptorSetLayout layout) {
    if (currentPool == VK_NULL_HANDLE) {
        VkDescriptorPool pool = getPool();
        currentPool = pool;
        usedPools.push_back(pool);
    }

    std::vector<VkDescriptorSetLayout> layouts(setCount, layout);
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = currentPool;
    allocateInfo.descriptorSetCount = setCount;
    allocateInfo.pSetLayouts = layouts.data();

    VkResult allocResult = vkAllocateDescriptorSets(device, &allocateInfo, sets);
    bool needReallocate = false;

    switch (allocResult) {
		case VK_SUCCESS:
			return;
		case VK_ERROR_FRAGMENTED_POOL:
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			needReallocate = true;
			break;
		default:
			throw std::runtime_error("failed to create descriptor sets!");
    }

    if (needReallocate) {
        currentPool = getPool();
        usedPools.push_back(currentPool);
        if (vkAllocateDescriptorSets(device, &allocateInfo, sets) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor sets after trying to make a new pool!");
        }
    }
}

VkDescriptorPool DescriptorAllocator::getPool() {
    if (freePools.size() > 0) {
        VkDescriptorPool pool = freePools.back();
        freePools.pop_back();
        return pool;
    }
    else {
        VkDescriptorPool pool{};

        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(DEFAULT_DESCRIPTOR_POOL_SIZE);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(DEFAULT_DESCRIPTOR_POOL_SIZE);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(DEFAULT_DESCRIPTOR_POOL_SIZE);

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        return pool;
    }
}
