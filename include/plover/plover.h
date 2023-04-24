#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef _WIN32
# define EXPORT __declspec( dllexport )
#else
# define EXPORT
#endif

#define internal_func static
#define global_var static
#define local_persist static

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t  i64;
typedef int32_t  i32;
typedef int16_t  i16;
typedef int8_t   i8;

typedef double   f64;
typedef float    f32;

#define Kilobytes(kb) ((uint64_t) kb * 1024)
#define Megabytes(mb) ((uint64_t) mb * 1024 * 1024)
#define Gigabytes(gb) ((uint64_t) gb * 1024 * 1024 * 1024)

enum RenderMessageTag {
	CREATE_MESH,
	CREATE_MATERIAL,
};

struct CreateMeshData {
	const char *path;
	u32 materialID;
};

struct RenderMessage {
	RenderMessageTag tag;
	u32 id; // NOTE(oliver): Related output messages will have this ID

	union {
		CreateMeshData createMesh;
	} v;
};

enum RenderResultTag {
	MESH_CREATED,
	MATERIAL_CREATED,
};

struct MeshCreatedData {
	u32 meshID;
};

struct MaterialCreatedData {
	u32 materialID;
};

struct RenderResult {
	RenderResultTag tag;
	u32 in_id; // NOTE(oliver): ID of input message. 0 for no related message.

	union {
		 MeshCreatedData meshCreated;
		 MaterialCreatedData materialCreated;
	} v;
};


// NOTE(oliver): engine handles
struct Handles {
	void (*DEBUG_log)(const char *);

	// Rendering
	void (*pushRenderMessage)(RenderMessage);
	bool (*hasRenderResult)();
	RenderResult (*popRenderResult)();
};

struct GameMemory {
	bool initialized;

	u64   persistentStorageSize;
	void *persistentStorage;
};

typedef int f_gameUpdateAndRender(Handles*, GameMemory*);
