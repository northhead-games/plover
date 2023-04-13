#pragma once

#include "ttfRenderer.h"

struct TextureCreateInfo {
	void* pixels;
	u32 width;
	u32 height;
};

struct Texture {
	VkImage image;
	VmaAllocation allocation;
	VkImageView imageView;
	VkSampler sampler;
};

struct Bitmap {
	void* pixels;
	u32 width;
	u32 height;

	void drawGlyph(FT_Bitmap glyph, u32 x, u32 y);
};
