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

typedef uint32_t MeshID;
typedef size_t MaterialID;

// NOTE(oliver): engine handles
struct Handles {
	void (*DEBUG_log)(const char *);

	// Rendering
	MaterialID (*createMaterial)();
	MeshID (*createMesh)(const char *, MaterialID);
};

struct GameMemory {
	bool initialized;

	u64   persistentStorageSize;
	void *persistentStorage;
};

typedef int f_gameUpdateAndRender(Handles*, GameMemory*);
