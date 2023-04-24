#include "Texture.h"

void Bitmap::writeGrayscale(u8 value, u32 x, u32 y) {
	assert(x >= 0 && y >= 0);
	assert(x < width * stride());
	assert(x < height * stride());
	
	switch (format) {
		case G8:
			((u8 *)pixels)[index(x, y)] = value;
			break;
		case RGBA8:
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
