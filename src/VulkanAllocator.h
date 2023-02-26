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

	struct VulkanAllocator {
		VulkanContext* context;
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

		void init(VulkanContext* context);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void allocate(AllocationCreateInfo createInfo, Allocation& allocation);

		void free(Allocation& allocation);
	};
}