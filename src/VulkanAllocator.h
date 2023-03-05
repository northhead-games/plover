#pragma once

#include "includes.h"

namespace Plover {
	struct VulkanContext;

	struct CreateBufferInfo {
		VkDeviceSize size;
		VkBufferUsageFlags usage;
		VkMemoryPropertyFlags properties;
        VmaAllocationCreateFlagBits vmaFlags;
	};

	struct CreateImageInfo {
		uint32_t width;
		uint32_t height;
		VkFormat format;
		VkImageTiling tiling;
		VkImageUsageFlags usage;
		VkMemoryPropertyFlags properties;
        VmaAllocationCreateFlagBits vmaFlags;
	};

	struct VulkanAllocator {
		VulkanContext* context;
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        VmaAllocator _allocator;

		void init(VulkanContext* context, VmaAllocatorCreateInfo &allocatorInfo);

		void createBuffer(CreateBufferInfo createInfo, VkBuffer& buffer, VmaAllocation& bufferAllocation);
        void destroyBuffer(VkBuffer buffer, VmaAllocation allocation);
		void createImage(CreateImageInfo createInfo, VkImage& buffer, VmaAllocation& bufferAllocation);
        void destroyImage(VkImage image, VmaAllocation allocation);
        void mapMemory(VmaAllocation allocation, void **ppData);
        void unmapMemory(VmaAllocation allocation);

        void getAllocationInfo(VmaAllocation allocation, VmaAllocationInfo& allocInfo);

        void cleanup();
    };
}