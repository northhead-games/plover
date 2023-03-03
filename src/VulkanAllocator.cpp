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

void VulkanAllocator::createBuffer(CreateBufferInfo createInfo,
	VkBuffer& buffer,
	Allocation& bufferAllocation) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = createInfo.size;
	bufferInfo.usage = createInfo.usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(context->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(context->device, buffer, &memoryRequirements);

	AllocationCreateInfo allocateInfo{};
	allocateInfo.requirements = memoryRequirements;
	allocateInfo.properties = createInfo.properties;
	allocate(allocateInfo, bufferAllocation);

	vkBindBufferMemory(context->device, buffer, bufferAllocation.memoryHandle, bufferAllocation.offset);
}

void VulkanAllocator::createImage(CreateImageInfo createImage, VkImage& image, Allocation& imageAllocation)
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

	if (vkCreateImage(context->device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(context->device, image, &memoryRequirements);

	AllocationCreateInfo allocateInfo{};
	allocateInfo.properties = createImage.properties;
	allocateInfo.requirements = memoryRequirements;

	allocate(allocateInfo, imageAllocation);

	vkBindImageMemory(context->device, image, imageAllocation.memoryHandle, imageAllocation.offset);
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
