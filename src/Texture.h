#pragma once

#include "includes.h"

enum BitmapFormat {
	G8, // 8-bit gray
};

struct Bitmap {
	void* pixels;
	u32 width;
	u32 height;
	BitmapFormat format;
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

