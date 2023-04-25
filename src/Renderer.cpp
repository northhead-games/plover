#include "Renderer.h"

#include "VulkanContext.h"
#include "includes.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <obj/tiny_obj_loader.h>

void Renderer::init() {
	context = new VulkanContext;
	context->initWindow();
	context->initVulkan();
}

bool Renderer::render() {
  return context->render();
}

void Renderer::cleanup() {
	context->cleanup();
	delete this->context;
}

void Renderer::processCommand(RenderCommand inCmd) {
	u32 cmdID = inCmd.id;

	switch (inCmd.tag) {
		case CREATE_MESH: {
			CreateMeshData data = inCmd.v.createMesh;

			Mesh mesh;

			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::string warn, err;

			if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, data.path)) {
				throw std::runtime_error(warn + err);
			}

			std::unordered_map<Vertex, uint32_t> uniqueVertices{};

			for (const auto& shape : shapes) {
				for (const auto& index : shape.mesh.indices) {
					Vertex vertex{};

					vertex.pos = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.texCoord = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1] // Flip to conform to OBJ
					};

					vertex.color = { 1.0f, 1.0f, 1.0f };

					if (uniqueVertices.count(vertex) == 0) {
						uniqueVertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
						mesh.vertices.push_back(vertex);
					}

					mesh.indices.push_back(uniqueVertices[vertex]);
				}
			}

			RenderMessage message{MESH_CREATED, cmdID};
			message.v.meshCreated.meshID = context->addMesh(
				mesh.vertices,
				mesh.indices,
				data.materialID);
			messageQueue.push(message);
			break;
		}
		case CREATE_MATERIAL: {
			RenderMessage message{MATERIAL_CREATED, cmdID};
			message.v.materialCreated.materialID = context->createMaterial();
			messageQueue.push(message);
			break;
		}
		case SET_MESH_TRANSFORM: {
			SetMeshTransformData meshTransformData = inCmd.v.setMeshTransform;
			Mesh* mesh = context->meshes[meshTransformData.meshID];
			mesh->transform = meshTransformData.transform;
			break;
		}
	}
}

void Renderer::processCommands() {
	while (commandQueue.hasMessage()) {
		processCommand(commandQueue.pop());
	}
}
