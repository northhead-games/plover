#include "includes.h"

#include "Renderer.h"
#include "ttfRenderer.h"

#include "plover.cpp"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/stat.h>

#define LINUX_GAME_SO "../libProjectG.so"
#define GAME_UPDATE_RENDER_FUNC "gameUpdateAndRender"

// OS-Independent Wrappers
// TODO(oliver): Handle errors instead of just asserting
void readFile(const char *path, u8 **buffer, u32 *bufferSize) {
	FILE *fp = fopen(path, "r");
	if (fp != NULL) {
		if (fseek(fp, 0L, SEEK_END) == 0) {
			*bufferSize = ftell(fp);
			assert(*bufferSize != -1);

			*buffer = new u8[sizeof(u8) * (*bufferSize + 1)];
			assert(fseek(fp, 0L, SEEK_SET) == 0);

		    *bufferSize = fread(*buffer, sizeof(u8), *bufferSize, fp);
			assert(ferror(fp) == 0);

			(*buffer)[(*bufferSize)++] = '\0';
		}
	}
	fclose(fp);
}

void DEBUG_log(const char *f, ...) {
	va_list args;
	va_start(args, f);
	vprintf(f, args);
	va_end(args);
}

// Shared Library Loading
struct linux_GameCode {
	long modificationTime;
	void *sharedObject;
	f_gameUpdateAndRender* updateAndRender;

};

internal_func int linux_guarStub(Handles *_h, GameMemory *_gm) {
	printf("Shared lib not loaded\n");
	return 0;
}

internal_func linux_GameCode linux_loadGameCode(linux_GameCode oldCode) {
	int res;

	struct stat dlStat = {};
	res = stat(LINUX_GAME_SO, &dlStat);
	assert((!res) && "Failed to get shared library attributes!");

	if (dlStat.st_mtim.tv_nsec != oldCode.modificationTime) {
		linux_GameCode newCode = {};
		newCode.modificationTime = dlStat.st_mtim.tv_nsec;
		newCode.updateAndRender = linux_guarStub;

		if (oldCode.sharedObject) {
			res = dlclose(oldCode.sharedObject);
			assert(res && "Failed to unload shared object!");
		}
		newCode.sharedObject = dlopen(LINUX_GAME_SO, RTLD_LAZY);
		if (newCode.sharedObject) {
			newCode.updateAndRender = (f_gameUpdateAndRender *) dlsym(newCode.sharedObject, GAME_UPDATE_RENDER_FUNC);

			if (!newCode.updateAndRender) {
				newCode.updateAndRender = linux_guarStub;
			}
		}
		return newCode;
	}
	return oldCode;
}

// Handles
internal_func Handles linux_createHandles() {
	Handles handles{};
	handles.DEBUG_log = DEBUG_log;
	handles.getTime = getTime;
	handles.getInputMessage = getInputMessage;
	handles.pushRenderCommand = pushRenderCommand;
	handles.hasRenderMessage = hasRenderMessage;
	handles.popRenderMessage = popRenderMessage;
	return handles;
}

// Memory
internal_func GameMemory linux_createMemory() {
	GameMemory memory{};
	memory.initialized = false;
	memory.persistentStorageSize = Megabytes(64);
	memory.persistentStorage = calloc(1, memory.persistentStorageSize);

	return memory;
}

internal_func void linux_destroyMemory(GameMemory memory) {
	free(memory.persistentStorage);
}

int main() {
	linux_GameCode game {};
	game = linux_loadGameCode(game);
	Handles handles = linux_createHandles();
	GameMemory memory = linux_createMemory();

	// Initialize font rendering
	if (!ttfInit()) {
		exit(-1);
	}

	renderer.init();
	while (renderer.render()) {
		//memory.mousePosition = mousePosition;
		game.updateAndRender(&handles, &memory);
		renderer.processCommands();
	}
	renderer.cleanup();

	linux_destroyMemory(memory);
	return 0;
}
