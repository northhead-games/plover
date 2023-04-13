#include "ttfRenderer.h"

global_var FT_Library library;
global_var FT_Face    face;

bool ttf_init() {
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
