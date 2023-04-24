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

void Renderer::processMessage(RenderMessage inMsg) {
	switch (inMsg.tag) {
		case CREATE_MESH: {
			CreateMeshData data = inMsg.v.createMesh;

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

			RenderResult result{MESH_CREATED};
			result.v.meshCreated.meshID = context->addMesh(
				mesh.vertices,
				mesh.indices,
				data.materialID);
			resultQueue.push(result);
			break;
		}
		case CREATE_MATERIAL: {
			RenderResult result{MATERIAL_CREATED};
			result.v.materialCreated.materialID = context->createMaterial();
			resultQueue.push(result);
			break;
		}
	}
}

void Renderer::processMessages() {
	while (msgQueue.hasMessage()) {
		processMessage(msgQueue.pop());
	}
}

// void Renderer::setMeshTransform(MeshID meshId, Transform transform) {
// 	Mesh* mesh = context->meshes[meshId];

// 	mesh->transform = glm::mat4(1);
// 	mesh->transform = glm::scale(mesh->transform, { transform.scale.x, transform.scale.y, transform.scale.z });
// 	mesh->transform = glm::rotate(mesh->transform, transform.rotate.x, { 1.0f, 0.0f, 0.0f });
// 	mesh->transform = glm::rotate(mesh->transform, transform.rotate.y, { 0.0f, 1.0f, 0.0f });
// 	mesh->transform = glm::rotate(mesh->transform, transform.rotate.z, { 0.0f, 0.0f, 0.1f });
// 	mesh->transform = glm::translate(mesh->transform, { transform.translate.x, transform.translate.y, transform.translate.z });
// }
