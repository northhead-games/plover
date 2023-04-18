// Internal OS-Independent functions
#pragma once
#include "plover/plover.h"

// Read entire contents of file into buffer and return buffer
void readFile(const char *path, u8 **buffer, u64 *bufferSize);
