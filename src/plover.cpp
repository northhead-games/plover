// Platform independent functions
// Globals

global_var glm::vec2 mousePosition = glm::vec2(640.0f, 360.0f);
global_var Renderer renderer{};

// Input

void mouseCallback(GLFWwindow* window, double position_x, double position_y) {
	mousePosition = glm::vec2(position_x, position_y);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
#ifndef NDEBUG
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
#endif
}

void clickCallback(GLFWwindow* window, int button, int action, int mods) {
#ifndef NDEBUG
	if (action == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
#endif
}

internal_func bool isKeyDown(Key key) {
	return glfwGetKey(renderer.context->window, keyToGLFW(key)) == GLFW_PRESS;
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
