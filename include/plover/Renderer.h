#pragma once

#include<string>
#include<vector>

typedef uint32_t MeshID;

namespace Plover {
    struct VulkanContext;

	struct Renderer {
        VulkanContext* context;

        void init();
		void run();
        MeshID loadModel(const std::string name);
	};
}