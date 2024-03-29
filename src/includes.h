#pragma once

// GLFW
#define GLFW_INCLUDE_VULKAN
#define GLFW_FORCE_DEPTH_ZERO_TO_ONE
#include <GLFW/glfw3.h>

// VMA
#include <vma/vk_mem_alloc.h>

// Freetype
#include "ft2build.h"
#include FT_FREETYPE_H

#include <chrono>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cstring>
#include <set>
#include <map>
#include <optional>
#include <limits>
#include <fstream>
#include <array>

#include "plover/plover.h"
#include "plover_int.h"
