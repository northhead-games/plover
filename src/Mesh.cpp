#include "Mesh.h"
#include "VulkanContext.h"

void Mesh::updateUniformBuffer(uint32_t currentImage) {
	MeshUniform ubo{};
	ubo.model = transform;

	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void Mesh::createUniform(VulkanContext& context) {
	// Create Uniform Buffer
	VkDeviceSize bufferSize = sizeof(MeshUniform);

	uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBufferAllocations.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		CreateBufferInfo createInfo{};
		createInfo.size = bufferSize;
		createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		createInfo.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		createInfo.vmaFlags = static_cast<VmaAllocationCreateFlagBits>(
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);

		VmaAllocationInfo allocInfo = {};
		context.createBuffer(createInfo, uniformBuffers[i], uniformBufferAllocations[i]);
		vmaGetAllocationInfo(context.allocator, uniformBufferAllocations[i], &allocInfo);
		uniformBuffersMapped[i] = allocInfo.pMappedData;
	}

	// Create Uniform Descriptor Set
	uniformDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	context.descriptorAllocator.allocate(MAX_FRAMES_IN_FLIGHT, uniformDescriptorSets.data(), context.meshDescriptorSetLayout);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(MeshUniform);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = uniformDescriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(context.device, 1, &descriptorWrite, 0, nullptr);
	}
}

void Mesh::cleanup(VulkanContext& context) {
	vmaDestroyBuffer(context.allocator, vertexBuffer, vertexAllocation);
	vmaDestroyBuffer(context.allocator, indexBuffer, indexAllocation);

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vmaDestroyBuffer(context.allocator, uniformBuffers[i], uniformBufferAllocations[i]);
	}
}

void createVertexBuffer(VulkanContext& context,
						std::vector<Vertex> vertices,
						VkBuffer& buffer,
						VmaAllocation& allocation) {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	CreateBufferInfo stagingCreateInfo{};
	stagingCreateInfo.size = bufferSize;
	stagingCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingCreateInfo.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	stagingCreateInfo.vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

	VkBuffer stagingBuffer;
	VmaAllocation stagingBufferAllocation;
	context.createBuffer(stagingCreateInfo, stagingBuffer, stagingBufferAllocation);

	void* data;
	vmaMapMemory(context.allocator, stagingBufferAllocation, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vmaUnmapMemory(context.allocator, stagingBufferAllocation);

	CreateBufferInfo vertexCreateInfo{};
	vertexCreateInfo.size = bufferSize;
	vertexCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	vertexCreateInfo.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	vertexCreateInfo.vmaFlags = static_cast<VmaAllocationCreateFlagBits>(0);
	context.createBuffer(vertexCreateInfo, buffer, allocation);

	context.copyBuffer(stagingBuffer, buffer, bufferSize);
	vmaDestroyBuffer(context.allocator, stagingBuffer, stagingBufferAllocation);
}

void createIndexBuffer(VulkanContext& context,
					   std::vector<uint32_t> indices,
					   VkBuffer& buffer,
					   VmaAllocation& allocation) {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	CreateBufferInfo stagingCreateInfo{};
	stagingCreateInfo.size = bufferSize;
	stagingCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingCreateInfo.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	stagingCreateInfo.vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

	VkBuffer stagingBuffer;
	VmaAllocation stagingBufferAllocation;
	context.createBuffer(stagingCreateInfo, stagingBuffer, stagingBufferAllocation);

	void* data;
	vmaMapMemory(context.allocator, stagingBufferAllocation, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vmaUnmapMemory(context.allocator, stagingBufferAllocation);

	CreateBufferInfo indexCreateInfo{};
	indexCreateInfo.size = bufferSize;
	indexCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	indexCreateInfo.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	indexCreateInfo.vmaFlags = static_cast<VmaAllocationCreateFlagBits>(0);
	context.createBuffer(indexCreateInfo, buffer, allocation);

	context.copyBuffer(stagingBuffer, buffer, bufferSize);

	vmaDestroyBuffer(context.allocator, stagingBuffer, stagingBufferAllocation);
}

size_t createMesh(VulkanContext& context,
				  std::vector<Vertex> vertices,
				  std::vector<u32> indices,
				  size_t materialId) {
	local_persist size_t nextId = 1;
	Mesh* mesh = new Mesh;

	mesh->vertices = vertices;
	mesh->indices = indices;
	createVertexBuffer(context, vertices, mesh->vertexBuffer, mesh->vertexAllocation);
	createIndexBuffer(context, indices, mesh->indexBuffer, mesh->indexAllocation);
	mesh->createUniform(context);
	mesh->materialId = materialId;

	mesh->transform = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));
	size_t id = nextId;
	context.meshes[id] = mesh;
	nextId++;

	return id;
}

void createMeshDescriptorSetLayout(VulkanContext& context) {
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(context.device, &layoutInfo, nullptr, &context.meshDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("falied to create descriptor set layout!");
	}
}
