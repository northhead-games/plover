#include "plover/Renderer.h"

#include "VulkanContext.h"
#include "includes.h"

using namespace Plover;

void Renderer::run() {
	vulkanContext = new VulkanContext;
	vulkanContext->initWindow();
	vulkanContext->initVulkan();
	vulkanContext->mainLoop();
	vulkanContext->cleanup();
	delete vulkanContext;
}

