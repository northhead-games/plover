#pragma once

#include "VulkanContext.h"
#include "includes.h"

namespace Plover {
	class VulkanContext; // Forward Decl

	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
		}
	};

	class VertexBuffer {
	public:
		VkBuffer buffer;

		const std::vector<Vertex> vertices = {
			{{ 0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
			{{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		VertexBuffer(VulkanContext* context);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		~VertexBuffer();
	private:
		VulkanContext* context;

		VkDeviceMemory vertexBufferMemory;
	};
}