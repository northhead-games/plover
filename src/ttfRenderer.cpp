#include "ttfRenderer.h"

global_var FT_Library library;
global_var FT_Face    face;

global_var u8 *fontBuf;
global_var u64 fontBufSize;
global_var u8 *GPOSTable;

#define ttf_tag_size 4
#define ttf_u16_size 2
#define ttf_u32_size 4
#define ttf_offset32_size 4
#define ttf_read_tag(b, o) { (b + o)[0], (b + o)[1], (b + o)[2], (b + o)[3], '\0' }
#define ttf_read_u16(b, o) 256 * (b + o)[0] + (b + o)[1]
#define ttf_read_u32(b, o) ((b + o)[0]<<24) + ((b + o)[1]<<16) + ((b + o)[2]<<8) + (b + o)[3]

internal_func void loadGPOSTable() {
	assert(strcmp(FT_Get_Font_Format(face), "TrueType") == 0);

	u64 offset = ttf_u32_size;  // skip: sfntVersion
	u16 numTables = ttf_read_u16(fontBuf, offset);
	offset += ttf_u16_size * 4; // skip: numTables, searchRange, entrySelector, rangeShift

	const u64 tableRecordSize =
		ttf_tag_size +
		ttf_u32_size +
		ttf_offset32_size +
		ttf_u32_size;
	u64 gposOffset = 0;

	for (int tableIdx = 0; tableIdx < numTables; tableIdx++) {
		u8 tag[ttf_tag_size + 1] = ttf_read_tag(fontBuf, offset);

		if (strcmp((char *) tag, "GPOS") == 0) {
			gposOffset = ttf_read_u32(fontBuf, offset + ttf_tag_size + ttf_u32_size);
			break;
		}

		offset += tableRecordSize;
	}

	assert(gposOffset != 0);
	GPOSTable = fontBuf + gposOffset;
}

internal_func i32 getGlyphKerning(int glyph1, int glyph2) {
	assert(ttf_read_u16(GPOSTable, 0) == 1); // Major Version
	assert(ttf_read_u16(GPOSTable, ttf_u16_size) == 0); // Minor Version

	u16 lookupListOffset = ttf_read_u16(GPOSTable, ttf_u16_size * 4); // skip: majVer, minVer, scriptList, featureList
	u8 *lookupList = GPOSTable + lookupListOffset;
	u16 lookupCount = ttf_read_u16(lookupList, 0);

	for (int i=0; i<lookupCount; i++) {
		u16 lookupOffset = ttf_read_u16(lookupList, ttf_u16_size + ttf_u16_size * i);
		u8 *lookupTable = lookupList + lookupOffset;

		u16 lookupType = ttf_read_u16(lookupTable, 0);
		u16 subTableCount = ttf_read_u16(lookupTable, ttf_u16_size * 2);
		u8 *subTableOffsets = lookupTable + ttf_u16_size * 3;
		if (lookupType != 2) { // Pair Adjustment Subtable
			continue;
		}

		for (int sti=0; sti<subTableCount; sti++) {
			u16 subtableOffset = ttf_read_u16(subTableOffsets, 2 * ttf_u16_size);
			u8 *table = lookupTable + subtableOffset;
			u16 posFormat = ttf_read_u16(table, 0);
			u16 coverageOffset = ttf_read_u16(table, ttf_u16_size);

			switch (posFormat) {
				case 1: { // Adjustments for Glyph Pairs
					
				}
				case 2: { // Class Pair Adjustment
					u16 valueFormat1 = ttf_read_u16(table, 2 * ttf_u16_size);
					u16 valueFormat2 = ttf_read_u16(table, 3 * ttf_u16_size);
					if (valueFormat1 == 4 && valueFormat2 == 0) { // Only support horizontal advance adjustment
						u16 classDef1Offset = ttf_read_u16(table, 4 * ttf_u16_size);
						u16 classDef2Offset = ttf_read_u16(table, 5 * ttf_u16_size);
					}
				}
			}
		}
	}
	return 0;
}

bool ttfInit() {
	int error;

	error = FT_Init_FreeType( &library );
	if (error) {
		printf("Could not load FreeType!\n");
		return false;
	}

	readFile("../resources/fonts/Marco.ttf", &fontBuf, &fontBufSize);
	error = FT_New_Memory_Face( library,
						 fontBuf,
						 fontBufSize,
						 0,
						 &face);
	if ( error == FT_Err_Unknown_File_Format ) {
		printf("Font format unsupported!\n");
		return false;
	} else if ( error ) {
		printf("Error reading font file!\n");
		return false;
	}

	error = FT_Set_Char_Size( face, 
							  0, /* char width (0 = same as height) */
							  30 * 64, /* char heigth in 1/64 of pt */
							  96, /* Horizontal dpi (mm) */
							  96 /* Vertical dpi (mm) */
							 );

	loadGPOSTable();

	return true;
}

internal_func void drawGlyph(Bitmap& bitmap, FT_Bitmap& glyph, u32 x, u32 y) {
	assert(glyph.pixel_mode == FT_PIXEL_MODE_GRAY);

	for (u32 row = 0; row < glyph.rows; row++) {
		for (u32 col = 0; col < glyph.width; col++) {
			bitmap.writeGrayscale(glyph.buffer[row * glyph.width + col], x + col, y + row);
		}
	}
}

#define FONT_SCALE 26.6

void drawGlyphs(const char *text, Bitmap& bitmap) {
	FT_GlyphSlot slot = face->glyph;
	u32 pen_x = 16, pen_y = face->ascender / FONT_SCALE;
	int error;

	FT_UInt previous = 0;

	u32 idx = 0;
	u32 prevGlyphIdx = 0, glyphIdx = 0;
	while (text[idx] != '\0') {
		// NOTE(oliver): equivalent to FT_Get_Char_Idx + FT_Load_Glyph + FT_Render_Glyph
		// Renders to anti-aliased bitmap
		error = FT_Load_Char( face, text[idx], FT_LOAD_RENDER);
		glyphIdx = slot->glyph_index;

		getGlyphKerning(prevGlyphIdx, glyphIdx);

		assert(!error);

		if (pen_x + slot->metrics.width / FONT_SCALE > bitmap.width) {
			pen_y += face->height / FONT_SCALE;
			pen_x = 16;
		}

		drawGlyph(bitmap,
				  slot->bitmap,
				  pen_x + slot->bitmap_left,
				  pen_y - slot->bitmap_top );

		pen_x += slot->advance.x >> 6;
		pen_y += slot->advance.y >> 6;

		prevGlyphIdx = glyphIdx;
		idx++;
	}
	
}
