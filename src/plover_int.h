#pragma once
#include "includes.h"

// Read entire contents of file into buffer and return buffer
void readFile(const char *path, u8 **buffer, u32 *bufferSize);
void mouseCallback(GLFWwindow* window, double position_x, double position_y);

inline i32 keyToGLFW(Key glfwKey) {
	switch (glfwKey) {
		case KEY_W: return GLFW_KEY_W;
		case KEY_A: return GLFW_KEY_A;
		case KEY_S: return GLFW_KEY_S;
		case KEY_D: return GLFW_KEY_D;
		case KEY_SPACE: return GLFW_KEY_SPACE;
		case KEY_SHIFT: return GLFW_KEY_LEFT_SHIFT;
		default: assert(false && "Key not supported!");
	};
}
