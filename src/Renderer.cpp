#include "Renderer.h"

#include "includes.h"
#include "VulkanContext.h"
#include "Mesh.h"
#include "UI.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <obj/tiny_obj_loader.h>

void Renderer::init() {
	context = new VulkanContext;
	context->initWindow();
	context->initVulkan();
	createUIQuad(*context);
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
				int i = 0;
				for (const auto& index : shape.mesh.indices) {
					Vertex vertex{};

					vertex.pos = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};

					vertex.tangent = {};

					vertex.texCoord = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1] // NOTE(oliver): Flip to conform to OBJ
					};

					if (uniqueVertices.count(vertex) == 0) {
						uniqueVertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
						mesh.vertices.push_back(vertex);
					}

					mesh.indices.push_back(uniqueVertices[vertex]);

					i++;
					if ((i % 3) == 0) { // NOTE(oliver): Performed per tri
						u64 indicesSize = mesh.indices.size();
						u32 i0 = mesh.indices[indicesSize - 3];
						u32 i1 = mesh.indices[indicesSize - 2];
						u32 i2 = mesh.indices[indicesSize - 1];
						Vertex &v0 = mesh.vertices[i0];
						Vertex &v1 = mesh.vertices[i1];
						Vertex &v2 = mesh.vertices[i2];
						glm::vec3 v0v1 = v1.pos - v0.pos;
						glm::vec3 v0v2 = v2.pos - v0.pos;

						glm::vec2 deltaUV1 = v1.texCoord - v0.texCoord;
						glm::vec2 deltaUV2 = v2.texCoord - v0.texCoord;
						float k = 1 / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

						glm::vec3 tangent = glm::vec3(
							k * (deltaUV2.y * v0v1.x - deltaUV1.y * v0v2.x),
							k * (deltaUV2.y * v0v1.y - deltaUV1.y * v0v2.y),
							k * (deltaUV2.y * v0v1.z - deltaUV1.y * v0v2.z));

						v0.tangent = tangent;
						v1.tangent = tangent;
						v2.tangent = tangent;
						// glm::mat2x2 UV(deltaUV2.y, -deltaUV1.y, -deltaUV2.x, deltaUV1.x);
						// glm::mat2x3 E(v1.pos - v0.pos, v2.pos - v0.pos);
						// glm::mat2x3 TB = k*E*UV;

						// v0.tangent += TB[0];
						// v1.tangent += TB[0];
						// v2.tangent += TB[0];
					}
				}
			}

			RenderMessage message{MESH_CREATED, cmdID};
			message.v.meshCreated.meshID = createMesh(
				*context,
				mesh.vertices,
				mesh.indices,
				data.materialID);
			messageQueue.push(message);
			break;
		}
		case CREATE_MATERIAL: {
			CreateMaterialData materialData = inCmd.v.createMaterial;
			const char *textureName = materialData.textureName;
			const char *normalName = materialData.normalName;

			RenderMessage message{MATERIAL_CREATED, cmdID};
			message.v.materialCreated.materialID = createMaterial(*context, loader, textureName, normalName);
			messageQueue.push(message);
			break;
		}
		case SET_MESH_TRANSFORM: {
			SetMeshTransformData meshTransformData = inCmd.v.setMeshTransform;
			Mesh* mesh = context->meshes[meshTransformData.meshID];
			mesh->transform = meshTransformData.transform;
			break;
		}
		case SET_CAMERA: {
			SetCameraData cameraData = inCmd.v.setCamera;
			context->camera = cameraData.camera;
			break;
		}
	}
}

void Renderer::processCommands() {
	while (commandQueue.hasMessage()) {
		processCommand(commandQueue.pop());
	}
}
