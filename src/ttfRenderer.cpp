#include "ttfRenderer.h"

global_var FT_Library library;
global_var FT_Face    face;

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
							  16 * 64, /* char heigth in 1/64 of pt */
							  96, /* Horizontal dpi (mm) */
							  96 /* Vertical dpi (mm) */
							 );

	return true;
}

internal_func void drawGlyph(Bitmap& bitmap, FT_Bitmap& glyph, u32 x, u32 y) {
	for (int row = 0; row < glyph.rows; row++) {
	}
}

void drawGlyphs(const char *text, Bitmap& bitmap) {
	FT_GlyphSlot slot = face->glyph;
	u32 pen_x = 0, pen_y = 0;
	int error;

	u32 idx = 0;
	while (text[idx] != '\0') {
		// NOTE(oliver): equivalent to FT_Get_Char_Idx + FT_Load_Glyph + FT_Render_Glyph
		// Renders to anti-aliased bitmap
		error = FT_Load_Char( face, text[idx], FT_LOAD_RENDER);

		assert(!error);

		drawGlyph(bitmap,
				  slot->bitmap,
				  pen_x + slot->bitmap_left,
				  pen_y + slot->bitmap_top );

		pen_x += slot->advance.x >> 6;

		idx++;
	}
}
