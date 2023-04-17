#include "ttfRenderer.h"

global_var FT_Library library;
global_var FT_Face    face;

// typedef unsigned long
// (*FT_Stream_IoFunc)( FT_Stream       stream,
//                      unsigned long   offset,
//                      unsigned char*  buffer,
//                      unsigned long   count );

#define ttf_u16(b, o) 255 * (b + o)[0] + (b + o)[1]

internal_func void loadGPOSTable() {
	assert(strcmp(FT_Get_Font_Format(face), "TrueType") == 0);

	FT_Stream stream = face->stream;
	u16 numTables = ttf_u16(stream->base, 4);
}

bool ttfInit() {
	int error;

	error = FT_Init_FreeType( &library );
	if (error) {
		printf("Could not load FreeType!\n");
		return false;
	}
	// NOTE(oliver): This can be switched to FT_New_Memory_Face when we have an asset loading system
	error = FT_New_Face( library,
						 "../resources/fonts/Marco.ttf",
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

	FT_Bool use_kerning = FT_HAS_KERNING( face );
	FT_UInt previous = 0;

	if (use_kerning) {
		printf("blah\n");
	}

	u32 idx = 0;
	while (text[idx] != '\0') {
		// NOTE(oliver): equivalent to FT_Get_Char_Idx + FT_Load_Glyph + FT_Render_Glyph
		// Renders to anti-aliased bitmap
		error = FT_Load_Char( face, text[idx], FT_LOAD_RENDER);

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

		idx++;
	}
	
}
