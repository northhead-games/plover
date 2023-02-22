#include "plover/Renderer.h"
#include "includes.h"

using namespace Plover;

void Renderer::run() {
	vulkanContext.initWindow();
	vulkanContext.initVulkan();
	vulkanContext.mainLoop();
	vulkanContext.cleanup();
}

