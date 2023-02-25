#include "plover/Renderer.h"

#include "VulkanContext.h"
#include "includes.h"

using namespace Plover;

void Renderer::run() {
	VulkanContext vulkanContext;
	vulkanContext.initWindow();
	vulkanContext.initVulkan();
	vulkanContext.mainLoop();
	vulkanContext.cleanup();
}

