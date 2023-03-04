#pragma once

#include<string>
#include<vector>


namespace Plover {
    struct VulkanContext;

    typedef uint32_t MeshID;
    typedef size_t MaterialID;

	struct Renderer {
        VulkanContext* context;

        void init();
		void run();
        MaterialID createMaterial();
        MeshID loadModel(const std::string name, MaterialID materialId);
	};
}