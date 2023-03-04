#pragma once

#include "includes.h"

namespace Plover {
	struct VulkanContext;
    struct Block;

	struct Allocation {
		VkDeviceMemory memoryHandle;
		VkDeviceSize size;
		VkDeviceSize offset;
        bool needsReservedBlock;

        Allocation* next;
        Allocation* prev;
        Block* block;
	};

    struct Block {
        Allocation* head;
        VkDeviceMemory memoryHandle;
        VkDeviceSize size;
        bool reserved;
        Block* next;
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
        std::vector<Block*> blocks;

		void init(VulkanContext* context);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkDeviceSize getAllocationSize(uint32_t type);

        void checkAllocationError(VkResult res);

		void createBuffer(CreateBufferInfo createInfo, VkBuffer& buffer, Allocation& bufferAllocation);
		void createImage(CreateImageInfo createInfo, VkImage& buffer, Allocation& bufferAllocation);

		void allocate(AllocationCreateInfo createInfo, Allocation& allocation);

		void free(Allocation& allocation);

        void cleanup();

        Block *createMemoryBlock(int allocationMult, VkMemoryAllocateInfo &allocateInfo);

        void
        initializeAllocation(const AllocationCreateInfo &createInfo, Allocation &allocation, bool needsOwnBlock,
                             Block *block,
                             int nextOffset) const;
    };
}