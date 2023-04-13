#include "plover/plover.h"

#include "Renderer.h"
#include "ttfRenderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

using namespace Plover;

// Globals
global_var Renderer renderer{};

struct linux_GameCode {
	f_gameUpdateAndRender* updateAndRender;
};

internal_func int linux_guarStub(Handles *_h, GameMemory *_gm) {
	printf("Shared lib not loaded\n");
	return 0;
}

internal_func linux_GameCode linux_loadGameCode() {
	linux_GameCode result = {};
	result.updateAndRender = linux_guarStub;
	void *gameCodeSO = dlopen("../libProjectG.so", RTLD_LAZY);
	if (gameCodeSO) {
		result.updateAndRender = (f_gameUpdateAndRender*) dlsym(gameCodeSO, "gameUpdateAndRender");
		if (!result.updateAndRender) result.updateAndRender = linux_guarStub;
	}

	return result;
}

// Handles
void linux_DEBUG_log(const char *str) {
	printf("%s", str);
}

MaterialID linux_createMaterial() {
	return renderer.createMaterial();
}

MeshID linux_createMesh(const char *path, MaterialID material) {
	std::string path_str(path);
	return renderer.loadModel(path_str, material);
}

internal_func Handles linux_createHandles() {
	Handles handles{};
	handles.DEBUG_log = linux_DEBUG_log;
	handles.createMaterial = linux_createMaterial;
	handles.createMesh = linux_createMesh;
	return handles;
}

internal_func GameMemory linux_createMemory() {
	GameMemory memory{};
	memory.initialized = false;
	memory.persistentStorageSize = Gigabytes(46);
	memory.persistentStorage = calloc(1, memory.persistentStorageSize);

	return memory;
}

internal_func void linux_destroyMemory(GameMemory memory) {
	free(memory.persistentStorage);
}

int main() {
	linux_GameCode game = linux_loadGameCode();
	Handles handles = linux_createHandles();
	GameMemory memory = linux_createMemory();

	// Initialize font rendering
	if (!ttf_init()) {
		exit(-1);
	}

	renderer.init();
	while (renderer.render()) {
		game.updateAndRender(&handles, &memory);
	}
	renderer.cleanup();

	linux_destroyMemory(memory);
	return 0;
}
