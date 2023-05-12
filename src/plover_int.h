#pragma once
#include "includes.h"

// Read entire contents of file into buffer and return buffer
void readFile(const char *path, u8 **buffer, u32 *bufferSize);
void DEBUG_log(const char *f, ...);

// Input callbacks
void mouseCallback(GLFWwindow* window, double position_x, double position_y);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void clickCallback(GLFWwindow* window, int button, int action, int mods);

inline i32 keyToGLFW(Key key) {
	switch (key) {
		case KEY_W: return GLFW_KEY_W;
		case KEY_A: return GLFW_KEY_A;
		case KEY_S: return GLFW_KEY_S;
		case KEY_D: return GLFW_KEY_D;
		case KEY_SPACE: return GLFW_KEY_SPACE;
		case KEY_SHIFT: return GLFW_KEY_LEFT_SHIFT;
		default: assert(false && "Key not supported!");
	};
	return 0;
}

inline Key GLFWToKey(i32 glfwKey) {
	switch (glfwKey) {
		case GLFW_KEY_W: return KEY_W;
		case GLFW_KEY_A: return KEY_A;
		case GLFW_KEY_S: return KEY_S;
		case GLFW_KEY_D: return KEY_D;
		case GLFW_KEY_SPACE: return KEY_SPACE;
		case GLFW_KEY_LEFT_SHIFT: return KEY_SHIFT;
		default: return KEY_NONE;
	};
}
