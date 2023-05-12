#include "UI.h"
#include "VulkanContext.h"

void UIQuad::createDescriptorSets(VulkanContext& context) {
	uniformDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	context.descriptorAllocator.allocate(MAX_FRAMES_IN_FLIGHT,
										 uniformDescriptorSets.data(),
										 context.uiDescriptorSetLayout);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture.imageView;
		imageInfo.sampler = texture.sampler;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = uniformDescriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(context.device, 1, &descriptorWrite, 0, nullptr);
	}
}

void createUIDescriptorSetLayout(VulkanContext& context) {
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 0;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &samplerLayoutBinding;

	if (vkCreateDescriptorSetLayout(
			context.device,
			&layoutInfo,
			nullptr,
			&context.uiDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("falied to create ui descriptor set layout!");
	}
}

void createUIPipeline(VulkanContext& context) {
	PipelineCreateInfo createInfo{};
	createInfo.useDepthBuffer = false;
	createInfo.doCulling = true;
	createInfo.subpass = 1; // UI Subpass
	createInfo.vertexShaderPath = "../resources/spirv/ui_vert.spv";
	createInfo.fragmentShaderPath = "../resources/spirv/ui_frag.spv";
	createInfo.descriptorSetLayoutCount = 1;
	createInfo.pDescriptorSetLayouts = &context.uiDescriptorSetLayout;

	context.createGraphicsPipeline(createInfo, context.uiPipeline, context.uiPipelineLayout);
}

size_t createUIQuad(VulkanContext& context) {
	static size_t nextId = 0;
	UIQuad quad{};

	Bitmap bitmap{};
	bitmap.width = 640;
	bitmap.height = 360;
	bitmap.pixels = new u8[bitmap.width * bitmap.height * 4] { 0 };
	bitmap.format = RGBA8;

	TextureCreateInfo createInfo{};
	createInfo.bitmap = bitmap;

	for (int x = 0; x < bitmap.width; x++) {
		for (int y = 0; y < bitmap.height; y++) {
			if (x < 4 ||
				x > bitmap.width - 4 ||
				y < 4 ||
				y > bitmap.height - 4) {

				bitmap.writeGrayscale(255, x, y);
			}
		}
	}

	drawGlyphs("AvAvAv WoWo The Quick Brown Fox Jumped Over The Lazy Dog", bitmap);
	createTexture(context, createInfo, quad.texture);
	createVertexBuffer(context, quadVertices, quad.vertexBuffer, quad.vertexAllocation);
	createIndexBuffer(context, quadIndices, quad.indexBuffer, quad.indexAllocation);
	quad.createDescriptorSets(context);

	context.uiQuads[nextId] = quad;
	nextId++;

	delete[] (u8 *) bitmap.pixels;
	return nextId;
}
