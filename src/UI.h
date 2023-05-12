#pragma once

#include "includes.h"
#include "Texture.h"

struct VulkanContext;

struct UIQuad {
	VkBuffer vertexBuffer;
	VmaAllocation vertexAllocation;

	VkBuffer indexBuffer;
	VmaAllocation indexAllocation;

	Texture texture;

	std::vector<VkDescriptorSet> uniformDescriptorSets;

    void createDescriptorSets(VulkanContext& context);
};

void createUIPipeline(VulkanContext& context);

void createUIDescriptorSetLayout(VulkanContext& context);

size_t createUIQuad(VulkanContext& context);
