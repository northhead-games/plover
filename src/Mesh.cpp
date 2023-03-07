#include "Mesh.h"

using namespace Plover;

void Mesh::updateUniformBuffer(uint32_t currentImage) {
	MeshUniform ubo{};
	ubo.model = transform;

	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
