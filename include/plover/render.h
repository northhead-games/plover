struct Camera {
	glm::vec3 position;
	glm::vec3 direction;
};

enum RenderCommandTag {
	CREATE_MESH,
	CREATE_MATERIAL,
	SET_MESH_TRANSFORM,
	SET_CAMERA,
};

struct CreateMeshData {
	const char *path;
	u32 materialID;
};

struct CreateMaterialData {
	const char *texturePath;
	const char *normalPath;
};

struct SetMeshTransformData {
	u64 meshID;
	glm::mat4 transform;
};

struct SetCameraData {
	Camera camera;
};

struct RenderCommand {
	RenderCommandTag tag;
	u32 id; // NOTE(oliver): Related output messages will have this ID

	union {
		CreateMeshData createMesh;
		CreateMaterialData createMaterial;
		SetMeshTransformData setMeshTransform;
		SetCameraData setCamera;
	} v;
};

enum RenderMessageTag {
	MESH_CREATED,
	MATERIAL_CREATED,
};

struct MeshCreatedData {
	u32 meshID;
};

struct MaterialCreatedData {
	u32 materialID;
};

struct RenderMessage {
	RenderMessageTag tag;
	u32 cmdID; // NOTE(oliver): ID of input message. 0 for no related message.

	union {
		 MeshCreatedData meshCreated;
		 MaterialCreatedData materialCreated;
	} v;
};
