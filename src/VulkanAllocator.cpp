#include "VulkanContext.h"
#include "VulkanAllocator.h"

using namespace Plover;

void VulkanAllocator::init(VulkanContext* context) {
	this->context = context;
	vkGetPhysicalDeviceMemoryProperties(context->physicalDevice, &deviceMemoryProperties);
}

uint32_t VulkanAllocator::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) &&
			(deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type");
}

void VulkanAllocator::allocate(AllocationCreateInfo createInfo, Allocation& allocation)
{
	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = createInfo.requirements.size;
	allocateInfo.memoryTypeIndex = findMemoryType(createInfo.requirements.memoryTypeBits, createInfo.properties);

	allocation.size = createInfo.requirements.size;
	allocation.offset = 0;
	allocation.typeIndex = allocateInfo.memoryTypeIndex;

	VkResult res = vkAllocateMemory(context->device, &allocateInfo, nullptr, &allocation.memoryHandle);

	if (res == VK_ERROR_OUT_OF_DEVICE_MEMORY) {
		throw std::runtime_error("failed to allocate gpu memory, out of device memory!");
	}
	if (res == VK_ERROR_TOO_MANY_OBJECTS) {
		throw std::runtime_error("failed to allocate gpu memory, too many allocations!");
	}
	if (res != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate gpu memory!");
	}
}

void VulkanAllocator::free(Allocation& allocation) {
	vkFreeMemory(context->device, allocation.memoryHandle, nullptr);
}
