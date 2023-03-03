#pragma once

#include "includes.h"

namespace Plover {
	struct VulkanContext;

	struct Allocation {
		VkDeviceMemory memoryHandle;
		VkDeviceSize size;
		VkDeviceSize offset;
		uint32_t typeIndex;
	};

	struct AllocationCreateInfo {
		VkMemoryRequirements requirements;
		VkMemoryPropertyFlags properties;
	};

	struct CreateBufferInfo {
		VkDeviceSize size;
		VkBufferUsageFlags usage;
		VkMemoryPropertyFlags properties;
	};

	struct CreateImageInfo {
		uint32_t width;
		uint32_t height;
		VkFormat format;
		VkImageTiling tiling;
		VkImageUsageFlags usage;
		VkMemoryPropertyFlags properties;
	};

	struct VulkanAllocator {
		VulkanContext* context;
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

		void init(VulkanContext* context);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void createBuffer(CreateBufferInfo createInfo, VkBuffer& buffer, Allocation& bufferAllocation);
		void createImage(CreateImageInfo createInfo, VkImage& buffer, Allocation& bufferAllocation);

		void allocate(AllocationCreateInfo createInfo, Allocation& allocation);

		void free(Allocation& allocation);
	};
}