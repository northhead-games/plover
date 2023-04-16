// PLOVER WIN32 PLATFORM LAYER
// All win32 code should be in here

#include "plover/plover.h"

#include "Renderer.h"
#include "ttfRenderer.h"

#include <windows.h>

using namespace Plover;

global_var Renderer renderer{};

struct win32_GameCode
{
	f_gameUpdateAndRender* updateAndRender;
};

internal_func int win32_guarStub(Handles *_h, GameMemory *_gm) {
	OutputDebugStringA("Shared lib not loaded!\n");
	return 0;
}

internal_func win32_GameCode win32_loadGameCode() {
	win32_GameCode result = {};
	result.updateAndRender = win32_guarStub;

	HMODULE gameCodeDLL = LoadLibraryA("../ProjectG.dll");
	if (gameCodeDLL) {
		result.updateAndRender = (f_gameUpdateAndRender*)GetProcAddress(gameCodeDLL, "gameUpdateAndRender");
		if (!result.updateAndRender) result.updateAndRender = win32_guarStub;
	}

	return result;
}


// Handles
void win32_DEBUG_log(const char *str) {
	OutputDebugStringA(str);
}

MaterialID win32_createMaterial() {
	return renderer.createMaterial();
}

MeshID win32_createMesh(const char *path, MaterialID material) {
	std::string path_str(path);
	return renderer.loadModel(path_str, material);
}

internal_func Handles win32_createHandles() {
	Handles handles{};
	handles.DEBUG_log = win32_DEBUG_log;
	handles.createMaterial = win32_createMaterial;
	handles.createMesh = win32_createMesh;
	return handles;
}

// Memory
internal_func GameMemory win32_createMemory() {
	GameMemory memory{};
	memory.initialized = false;
	memory.persistentStorageSize = Megabytes(64);
	memory.persistentStorage = VirtualAlloc(0, memory.persistentStorageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	return memory;
}

internal_func void win32_destroyMemory(GameMemory memory) {
	VirtualFree(memory.persistentStorage, 0, MEM_RELEASE);
}


int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
) {
	win32_GameCode game = win32_loadGameCode();
	Handles handles = win32_createHandles();
	GameMemory memory = win32_createMemory();

	if (!ttfInit()) {
		exit(-1);
	}

	renderer.init();
	while (renderer.render()) {
		game.updateAndRender(&handles, &memory);
	}
	renderer.cleanup();

	return 0;
}
