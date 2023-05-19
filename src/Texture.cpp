#include "Texture.h"
#include "VulkanContext.h"
#include "lapwing.h"

void Bitmap::writeGrayscale(u8 value, u32 x, u32 y) {
	assert(x >= 0 && y >= 0);
	assert(x < width * stride());
	assert(x < height * stride());
	
	switch (format) {
		case G8:
			((u8 *)pixels)[index(x, y)] = value;
			break;
		case RGBA8:
		case SRGBA8:
			((u8 *)pixels)[index(x, y)]     = 255;
			((u8 *)pixels)[index(x, y) + 1] = 255;
			((u8 *)pixels)[index(x, y) + 2] = 255;
			((u8 *)pixels)[index(x, y) + 3] += value;
			if (((u8 *)pixels)[index(x, y) + 3] > 255) {
				((u8 *)pixels)[index(x, y) + 3] = 255;
			}
			break;
	}
}

void Texture::cleanup(VulkanContext& context) {
	vkDestroySampler(context.device, sampler, nullptr);
	vkDestroyImageView(context.device, imageView, nullptr);
	vmaDestroyImage(context.allocator, image, allocation);
}

internal_func void createTextureSampler(VulkanContext& context, Texture& texture) {
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(context.physicalDevice, &supportedFeatures);
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(context.physicalDevice, &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	if (supportedFeatures.samplerAnisotropy == VK_TRUE) {
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	}
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(context.device, &samplerInfo, nullptr, &(texture.sampler)) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void createTexture(VulkanContext& context, TextureCreateInfo info, Texture& texture) {
	VkDeviceSize imageSize = info.bitmap.width * info.bitmap.height * info.bitmap.stride();
	VkFormat format = info.bitmap.vulkanFormat();

	VkBuffer stagingBuffer;
	VmaAllocation stagingBufferAllocation;

	CreateBufferInfo stagingCreateInfo{};
	stagingCreateInfo.size = imageSize;
	stagingCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingCreateInfo.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	stagingCreateInfo.vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

	context.createBuffer(stagingCreateInfo, stagingBuffer, stagingBufferAllocation);

	void* data;
	vmaMapMemory(context.allocator, stagingBufferAllocation, &data);
	memcpy(data, info.bitmap.pixels, static_cast<size_t>(imageSize));
	vmaUnmapMemory(context.allocator, stagingBufferAllocation);

	CreateImageInfo createInfo{};
	createInfo.width = info.bitmap.width;
	createInfo.height = info.bitmap.height;
	createInfo.format = format;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	createInfo.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	createInfo.vmaFlags = static_cast<VmaAllocationCreateFlagBits>(0);
	context.createImage(createInfo, texture.image, texture.allocation);

	// TODO(oliver): These all create and submit a new command buffer.  Should change this to use a single command buffer
	context.transitionImageLayout(texture.image,
								  format,
								  VK_IMAGE_LAYOUT_UNDEFINED,
								  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	context.copyBufferToImage(stagingBuffer,
							  texture.image,
							  static_cast<uint32_t>(info.bitmap.width),
							  static_cast<uint32_t>(info.bitmap.height));
	context.transitionImageLayout(texture.image,
								  format,
								  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
								  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vmaDestroyBuffer(context.allocator,
					 stagingBuffer,
					 stagingBufferAllocation);

	texture.imageView = context.createImageView(texture.image, format, VK_IMAGE_ASPECT_COLOR_BIT);
	createTextureSampler(context, texture);
}

void createImageTexture(VulkanContext& context, AssetLoader loader, Texture& texture, const char *name, BitmapFormat format) {
	ImageInfo info{};

	TextureCreateInfo createInfo{};
	createInfo.bitmap.pixels = loader.loadTexture(name, &info);
	createInfo.bitmap.width = info.width;
	createInfo.bitmap.height = info.height;
	createInfo.bitmap.format = format;

	createTexture(context, createInfo, texture);
}
