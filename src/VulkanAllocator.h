#pragma once

#include "includes.h"

namespace Plover {
	struct VulkanContext;
    struct Block;

	struct Allocation {
		VkDeviceMemory memoryHandle;
		VkDeviceSize size;
		VkDeviceSize offset;

        Allocation* next;
        Allocation* prev;
        Block* block;
	};

    struct Block {
        Allocation* head;
        VkDeviceMemory memoryHandle;
        VkDeviceSize size;
        uint32_t mapCounter;
        void* data;
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

        //Helper functions
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkDeviceSize getAllocationSize(uint32_t type);
        Block *createMemoryBlock(int allocationMult, VkMemoryAllocateInfo &allocateInfo);
        void initializeAllocation(const AllocationCreateInfo &createInfo, Block *block, int nextOffset,
                                  Allocation &allocation);

        void checkAllocationError(VkResult res);

		void createBuffer(CreateBufferInfo createInfo, VkBuffer& buffer, Allocation& bufferAllocation);
		void createImage(CreateImageInfo createInfo, VkImage& buffer, Allocation& bufferAllocation);

        void mapMemory(VkDevice device, Allocation allocation, void** ppData);
        void unmapMemory(VkDevice device, Allocation allocation);

		void allocate(AllocationCreateInfo createInfo, Allocation& allocation);
		void free(Allocation& allocation);

        void cleanup();
    };
}