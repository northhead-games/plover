// PLOVER WIN32 PLATFORM LAYER
// All win32 code should be in here

#include "plover/plover.h"

#include "Renderer.h"
#include "ttfRenderer.h"

#include "plover.cpp"

#include <windows.h>
#include <fileapi.h>

void readFile(const char *path, u8 **buffer, u32 *bufferSize) {
	HANDLE hFile = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	*bufferSize = GetFileSize(hFile, (LPDWORD) bufferSize);
	assert(*bufferSize != NULL);

	*buffer = new u8[sizeof(u8) * (*bufferSize + 1)];
	assert(ReadFile(hFile, *buffer, *bufferSize, (LPDWORD) bufferSize, NULL));
	CloseHandle(hFile);
}

void DEBUG_log(const char *f, ...) {
	va_list args;
	va_start(args, f);
	char str[1024];
	vsprintf_s(str, 1024, f, args);
	OutputDebugStringA(str);
	va_end(args);
}

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

internal_func Handles win32_createHandles() {
	Handles handles{};
	handles.DEBUG_log = DEBUG_log;
	handles.isKeyDown = isKeyDown;
	handles.pushRenderCommand = pushRenderCommand;
	handles.hasRenderMessage = hasRenderMessage;
	handles.popRenderMessage = popRenderMessage;
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

internal_func void win32_destroyMemory(GameMemory* memory) {
	VirtualFree(memory->persistentStorage, 0, MEM_RELEASE);
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
		memory.mousePosition = mousePosition;
		game.updateAndRender(&handles, &memory);
		renderer.processCommands();
	}
	renderer.cleanup();

	win32_destroyMemory(&memory);
	return 0;
}
