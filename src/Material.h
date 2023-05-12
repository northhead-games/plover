#pragma once

#include "includes.h"
#include "Texture.h"

struct VulkanContext;

struct Material {
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;

	Texture texture;
	Texture normalTexture;
	std::vector<VkDescriptorSet> descriptorSets;

	void createDescriptorSets(VulkanContext& context);
	void cleanup(VulkanContext& context);
};

size_t createMaterial(VulkanContext& context, const char *texturePath, const char *normalPath);
void createMaterialDescriptorSetLayout(VulkanContext& context);
