// Platform independent functions
// Globals

#include "MessageQueue.h"

global_var Renderer renderer{};
global_var MessageQueue<InputMessage> inputQueue;
global_var bool sendMouseMovements = true;
global_var bool isMouseReset = false;

// Input

double getTime() {
	return glfwGetTime();
}

void mouseCallback(
	GLFWwindow* window,
	double position_x,
	double position_y) {
	if (sendMouseMovements) {
		InputMessage message{MOUSE_MOVED};
		message.v.mouseMoved.mousePosition = Vec2(position_x, position_y);
		message.v.mouseMoved.resetPrevious = isMouseReset;
		inputQueue.push(message);

		isMouseReset = false;
	}
}

void keyCallback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mods) {
	InputMessage message{};
	switch (action) {
		case GLFW_PRESS: {
			message.tag = KEY_DOWN;
			message.v.keyDown.key = GLFWToKey(key);
			break;
		}
		case GLFW_RELEASE: {
			message.tag = KEY_UP;
			message.v.keyUp.key = GLFWToKey(key);
			break;
		}
	}
	inputQueue.push(message);

#ifndef NDEBUG
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		sendMouseMovements = false;
	}
#endif
}

void clickCallback(
	GLFWwindow* window,
	int button,
	int action,
	int mods) {
#ifndef NDEBUG
	if (action == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		sendMouseMovements = true;
		isMouseReset = true;
	}
#endif
}

internal_func bool isKeyDown(Key key) {
	return glfwGetKey(renderer.context->window, keyToGLFW(key)) == GLFW_PRESS;
}

internal_func InputMessage getInputMessage() {
	if (inputQueue.hasMessage()) {
		return inputQueue.pop();
	}
	return {NO_MESSAGE};
}

// Render

internal_func void pushRenderCommand(RenderCommand inMsg) {
	return renderer.commandQueue.push(inMsg);
}

internal_func bool hasRenderMessage() {
	return renderer.messageQueue.hasMessage();
}

internal_func RenderMessage popRenderMessage() {
	return renderer.messageQueue.pop();
}
