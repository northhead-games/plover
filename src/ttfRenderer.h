#pragma once

#include "freetype/ftfntfmt.h"
#include "freetype/ftotval.h"

#include "includes.h"
#include "Texture.h"

bool ttfInit();
void drawGlyphs(const char *text, Bitmap& bitmap);
