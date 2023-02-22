#pragma once

#include "VulkanContext.h"

namespace Plover {
	class Renderer {
	public:
		void run();
	private:
		VulkanContext vulkanContext;
	};
}