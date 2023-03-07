#pragma once

#include<string>
#include<vector>


namespace Plover {
	struct VulkanContext;

	typedef uint32_t MeshID;
	typedef size_t MaterialID;

	struct Transform {
		struct Translate {
			float x;
			float y;
			float z;
		} translate;

		struct Rotation {
			float x;
			float y;
			float z;
		} rotate;

		struct Scale {
			float x;
			float y;
			float z;
		} scale;
	};

	struct Renderer {
		VulkanContext* context;

		void init();
		void run();
		MaterialID createMaterial();
		MeshID loadModel(const std::string name, MaterialID materialId);
		void setMeshTransform(MeshID mesh, Transform transform);
	};
}