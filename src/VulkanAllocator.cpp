#include "VulkanContext.h"
#include "VulkanAllocator.h"

using namespace Plover;

void VulkanAllocator::init(VulkanContext* context) {
	this->context = context;
	vkGetPhysicalDeviceMemoryProperties(context->physicalDevice, &deviceMemoryProperties);
    blocks.resize(deviceMemoryProperties.memoryTypeCount);

    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        blocks[i] = nullptr;
    }
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

VkDeviceSize VulkanAllocator::getAllocationSize(uint32_t type) {
    if (type >= deviceMemoryProperties.memoryTypeCount) {
        throw std::runtime_error("failed to find the allocation size of the given memory type");
    }

    return std::min((uint64_t) (256 * 1024 * 1024), ((uint64_t) deviceMemoryProperties.memoryHeaps[deviceMemoryProperties.memoryTypes[type].heapIndex].size) / 8);
}

void VulkanAllocator::initializeAllocation(const AllocationCreateInfo &createInfo, Block *block, int nextOffset,
                                           Allocation &allocation) {
    allocation.size = createInfo.requirements.size;
    allocation.offset = nextOffset;
    allocation.memoryHandle = block->memoryHandle;
    allocation.block = block;
    allocation.next = nullptr;
    allocation.prev = nullptr;
}

Block *VulkanAllocator::createMemoryBlock(int allocationMult, VkMemoryAllocateInfo &allocateInfo) {
    Block *block = (Block*) malloc(sizeof(Block));
    block->next = nullptr;
    block->data = nullptr;
    block->mapCounter = 0;
    block->size = getAllocationSize(allocateInfo.memoryTypeIndex) * allocationMult;
    allocateInfo.allocationSize = block->size;

    Allocation* sentinel = (Allocation*) malloc(sizeof(Allocation));
    sentinel->offset = 0;
    sentinel->size = 0;
    sentinel->prev = nullptr;
    sentinel->next = nullptr;

    block->head = sentinel;
    VkResult res = vkAllocateMemory(context->device, &allocateInfo, nullptr, &block->memoryHandle);
    checkAllocationError(res);

    return block;
}

void VulkanAllocator::checkAllocationError(VkResult res) {
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

    std::cout << "ALLOC BUFFER" << std::endl;
    std::cout << "Handle: " << bufferAllocation.memoryHandle << std::endl;
    std::cout << "Buffer Size: " << bufferAllocation.size << std::endl;
    std::cout << "Buffer Offset: " << bufferAllocation.offset << std::endl << std::endl;

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

    std::cout << "ALLOC IMAGE" << std::endl;
    std::cout << "Handle: " << imageAllocation.memoryHandle << std::endl;
    std::cout << "Buffer Size: " << imageAllocation.size << std::endl;
    std::cout << "Buffer Offset: " << imageAllocation.offset << std::endl << std::endl;

	vkBindImageMemory(context->device, image, imageAllocation.memoryHandle, imageAllocation.offset);
}

void VulkanAllocator::mapMemory(VkDevice device, Allocation allocation, void **ppData) {
    if (allocation.block->mapCounter != 0) {
        allocation.block->mapCounter++;
    } else {
        allocation.block->mapCounter++;
        void *data;
        vkMapMemory(device, allocation.memoryHandle, 0, VK_WHOLE_SIZE, 0, &data);
        allocation.block->data = data;
    }
    *ppData = (void*) ((char*) allocation.block->data + (std::ptrdiff_t) allocation.offset);
}

void VulkanAllocator::unmapMemory(VkDevice device, Allocation allocation) {
    assert(allocation.block->mapCounter > 0);

    if (allocation.block->mapCounter == 1) {
        allocation.block->mapCounter--;
        vkUnmapMemory(device, allocation.memoryHandle);
        allocation.block->data = nullptr;
    } else {
        allocation.block->mapCounter--;
    }
}

void VulkanAllocator::allocate(AllocationCreateInfo createInfo, Allocation& allocation)
{
    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.memoryTypeIndex = findMemoryType(createInfo.requirements.memoryTypeBits, createInfo.properties);

    if (blocks[allocateInfo.memoryTypeIndex] == nullptr) {
        int allocationMult = 1;
        // If the default block is not big enough to fit the data.
        if (createInfo.requirements.size > getAllocationSize(allocateInfo.memoryTypeIndex)) {
            allocationMult = std::ceil((float) createInfo.requirements.size / (float) getAllocationSize(allocateInfo.memoryTypeIndex));
        }

        Block *block = createMemoryBlock(allocationMult, allocateInfo);
        block->next = blocks[allocateInfo.memoryTypeIndex];
        blocks[allocateInfo.memoryTypeIndex] = block;
    }

    {
        Block *block = blocks[allocateInfo.memoryTypeIndex];
        do {
            // If the block is not large enough for the allocation.
            if (block->size < createInfo.requirements.size) {
                block = block->next;
                continue;
            }

            Allocation *alloc = block->head;
            do {
                int nextOffset = alloc->offset + alloc->size;
                if ((alloc->offset + alloc->size) % createInfo.requirements.alignment != 0) {
                    nextOffset += (createInfo.requirements.alignment - ((alloc->offset + alloc->size) % createInfo.requirements.alignment));
                }

                // Exit early if we searched through enough of the block that the thing we are allocating can no longer fit.
                if (block->size - nextOffset < createInfo.requirements.size) {
                    break;
                }

                // If there is no allocation after the current one and there is enough space as the previous if passed.
                if (alloc->next == nullptr) {
                    std::cout << "Adding to the end of the block" << std::endl;
                    initializeAllocation(createInfo, block, nextOffset, allocation);

                    alloc->next = &allocation;
                    allocation.prev = alloc;

                    return;
                }

                // If there is enough space after an existing allocation place it there.
                if (alloc->next->offset - nextOffset >= createInfo.requirements.size) {
                    std::cout << "Putting in the middle of two allocations" << std::endl;
                    initializeAllocation(createInfo, block, nextOffset, allocation);

                    allocation.next = alloc->next;
                    allocation.next->prev = &allocation;
                    allocation.prev = alloc;
                    alloc->next = &allocation;

                    return;
                }

                alloc = alloc->next;
            } while (alloc != nullptr);
            block = block->next;
        } while (block != nullptr);
    }

    // If a suitable memory block was not found create a new block and assign the data.
    int allocationMult = 1;
    // If the current blocks are not big enough to fit the data.
    if (createInfo.requirements.size > getAllocationSize(allocateInfo.memoryTypeIndex)) {
        allocationMult = std::ceil((float) createInfo.requirements.size / (float) getAllocationSize(allocateInfo.memoryTypeIndex));
    }

    Block *block = createMemoryBlock(allocationMult, allocateInfo);
    block->next = blocks[allocateInfo.memoryTypeIndex];
    blocks[allocateInfo.memoryTypeIndex] = block;

    initializeAllocation(createInfo, block, 0, allocation);

    block->head->next = &allocation;
    allocation.prev = block->head;
}

void VulkanAllocator::free(Allocation& allocation) {
    std::cout << "FREE" << std::endl;
    std::cout << "Handle: " << allocation.memoryHandle << std::endl;
    std::cout << "Buffer Offset: " << allocation.offset << std::endl;

    allocation.prev->next = allocation.next;
    if (allocation.next != nullptr) {
        allocation.next->prev = allocation.prev;
    }

    allocation.prev = nullptr;
    allocation.next = nullptr;
    allocation.block = nullptr;
    allocation.offset = 0;
    allocation.memoryHandle = 0;
    allocation.size = 0;
}

void VulkanAllocator::cleanup() {
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        Block* block = blocks[i];
        while(block != nullptr) {
            std::free(block->head);
            vkFreeMemory(context->device, block->memoryHandle, nullptr);
            Block* next = block->next;
            std::free(block);
            block = next;
        }
    }
}
