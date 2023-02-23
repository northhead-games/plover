#pragma once


namespace Plover {
	class VulkanContext; // Forward Decl

	class Renderer {
	public:
		void run();
	private:
		VulkanContext* vulkanContext;
	};
}