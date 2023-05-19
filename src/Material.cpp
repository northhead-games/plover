#include "Material.h"
#include "VulkanContext.h"

void Material::createDescriptorSets(VulkanContext& context) {
	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	context.descriptorAllocator.allocate(MAX_FRAMES_IN_FLIGHT, descriptorSets.data(), context.materialDescriptorSetLayout);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorImageInfo texInfo{};
		texInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		texInfo.imageView = texture.imageView;
		texInfo.sampler = texture.sampler;

		VkDescriptorImageInfo nrmInfo{};
		nrmInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		nrmInfo.imageView = normalTexture.imageView;
		nrmInfo.sampler = normalTexture.sampler;

		VkWriteDescriptorSet descriptorWrites[2];
		descriptorWrites[0] = {};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pImageInfo = &texInfo;

		descriptorWrites[1] = {};
		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &nrmInfo;

		vkUpdateDescriptorSets(context.device, 2, descriptorWrites, 0, nullptr);
	}
}

void Material::cleanup(VulkanContext& context) {
	vkDestroyPipeline(context.device, pipeline, nullptr);
	vkDestroyPipelineLayout(context.device, pipelineLayout, nullptr);

	texture.cleanup(context);
	normalTexture.cleanup(context);
}

void createMaterialDescriptorSetLayout(VulkanContext& context) {
	VkDescriptorSetLayoutBinding layoutBindings[2];
	layoutBindings[0].binding = 0;
	layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	layoutBindings[0].descriptorCount = 1;
	layoutBindings[0].pImmutableSamplers = nullptr;
	layoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	layoutBindings[1].binding = 1;
	layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	layoutBindings[1].descriptorCount = 1;
	layoutBindings[1].pImmutableSamplers = nullptr;
	layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 2;
	layoutInfo.pBindings = layoutBindings;

	if (vkCreateDescriptorSetLayout(context.device,
									&layoutInfo,
									nullptr,
									&context.materialDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("falied to create descriptor set layout!");
	}
}

size_t createMaterial(VulkanContext& context,
					  AssetLoader loader,
					  const char *texturePath,
					  const char *normalPath) {
	local_persist size_t nextId = 1;
	Material material{};

	VkDescriptorSetLayout descriptorSetLayouts[3] = {
		context.globalDescriptorSetLayout,
		context.materialDescriptorSetLayout,
		context.meshDescriptorSetLayout
	};

	PipelineCreateInfo createInfo{};
	createInfo.useDepthBuffer = true;
	createInfo.doCulling = true;
	createInfo.subpass = 0; // Forward Rendering Subpass
	createInfo.vertexShaderPath = "../resources/spirv/vert.spv";
	createInfo.fragmentShaderPath = "../resources/spirv/frag.spv";
	createInfo.descriptorSetLayoutCount = 3;
	createInfo.pDescriptorSetLayouts = descriptorSetLayouts;

	context.createGraphicsPipeline(createInfo, material.pipeline, material.pipelineLayout);

	createImageTexture(context, loader, material.texture, texturePath, SRGBA8);
	createImageTexture(context, loader, material.normalTexture, normalPath, RGBA8);
	material.createDescriptorSets(context);

	size_t id = nextId;
	context.materials[id] = material;
	nextId++;

	return id;
}
