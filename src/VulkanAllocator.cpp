#include "VulkanContext.h"
#include "VulkanAllocator.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#pragma clang diagnostic pop

using namespace Plover;

void VulkanAllocator::init(VulkanContext* context, VmaAllocatorCreateInfo &allocatorInfo) {
	this->context = context;
	vkGetPhysicalDeviceMemoryProperties(context->physicalDevice, &deviceMemoryProperties);

    vmaCreateAllocator(&allocatorInfo, &_allocator);
}

void VulkanAllocator::createBuffer(CreateBufferInfo createInfo, VkBuffer& buffer, VmaAllocation& bufferAllocation) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = createInfo.size;
	bufferInfo.usage = createInfo.usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.requiredFlags = createInfo.properties;
    allocCreateInfo.flags = createInfo.vmaFlags;

    vmaCreateBuffer(_allocator, &bufferInfo, &allocCreateInfo, &buffer, &bufferAllocation, nullptr);
}

void VulkanAllocator::destroyBuffer(VkBuffer buffer, VmaAllocation allocation) {
    vmaDestroyBuffer(_allocator, buffer, allocation);
}

void VulkanAllocator::createImage(CreateImageInfo createImage, VkImage& image, VmaAllocation& imageAllocation)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = createImage.width;
	imageInfo.extent.height = createImage.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = createImage.format;
	imageInfo.tiling = createImage.tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = createImage.usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.requiredFlags = createImage.properties;
    allocCreateInfo.flags = createImage.vmaFlags;

    vmaCreateImage(_allocator, &imageInfo, &allocCreateInfo, &image, &imageAllocation, nullptr);
}

void VulkanAllocator::destroyImage(VkImage image, VmaAllocation allocation) {
    vmaDestroyImage(_allocator, image, allocation);
}


void VulkanAllocator::mapMemory(VmaAllocation allocation, void **ppData) {
    void* data;
    vmaMapMemory(_allocator, allocation, &data);
    *ppData = data;
};

void VulkanAllocator::unmapMemory(VmaAllocation allocation) {
    vmaUnmapMemory(_allocator, allocation);
};

void VulkanAllocator::getAllocationInfo(VmaAllocation allocation, VmaAllocationInfo &allocInfo) {
    vmaGetAllocationInfo(_allocator, allocation, &allocInfo);
}

void VulkanAllocator::cleanup() {
    vmaDestroyAllocator(_allocator);
}
