#pragma once

#include "includes.h"

enum BitmapFormat {
	G8, // 8-bit gray
	RGBA8, // 8-bit rgba
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
		}
	}

	inline VkFormat vulkanFormat() {
		switch (format) {
			case G8:
				return VK_FORMAT_R8_SRGB;
			case RGBA8:
				return VK_FORMAT_R8G8B8A8_SRGB;
		}
	}

	inline u32 index(u32 x, u32 y) {
		u8 stride = 1;
		if (format == RGBA8) {
			stride = 4;
		}
		return (y * width + x) * stride;
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

