#pragma once

// GLFW
#define GLFW_INCLUDE_VULKAN
#define GLFW_FORCE_DEPTH_ZERO_TO_ONE
#include <GLFW/glfw3.h>

// GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/matrix_transform.hpp>

// VMA
#include <vma/vk_mem_alloc.h>

// Freetype
#include "ft2build.h"
#include FT_FREETYPE_H
#include "freetype/ftfntfmt.h"

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
