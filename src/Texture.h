#pragma once

#include "includes.h"

enum BitmapFormat {
	G8, // 8-bit gray
	RGBA8, // 8-bit rgba
	SRGBA8, // 8-bit srgba
};

struct Bitmap {
	void *pixels;
	u32 width;
	u32 height;
	BitmapFormat format;

	inline u32 stride() {
		switch (format) {
			case G8:
				return 1;
			case RGBA8:
				return 4;
			case SRGBA8:
				return 4;
		}
	}

	inline VkFormat vulkanFormat() {
		switch (format) {
			case G8:
				return VK_FORMAT_R8_UNORM;
			case RGBA8:
				return VK_FORMAT_R8G8B8A8_UNORM;
			case SRGBA8:
				return VK_FORMAT_R8G8B8A8_SRGB;
		}
	}

	inline u32 index(u32 x, u32 y) {
		return (y * width + x) * stride();
	}

	void writeGrayscale(u8 value, u32 x, u32 y);
};

struct TextureCreateInfo {
	Bitmap bitmap;
};

struct Texture {
	VkImage image;
	VmaAllocation allocation;
	VkImageView imageView;
	VkSampler sampler;
};

