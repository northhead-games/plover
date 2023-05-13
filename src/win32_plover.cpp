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
	HMODULE gameCodeDLL;
	FILETIME dllFileTime;
	
	f_gameUpdateAndRender* updateAndRender;
};

internal_func int win32_guarStub(Handles *_h, GameMemory *_gm) {
	OutputDebugStringA("Shared lib not loaded!\n");
	return 0;
}

internal_func win32_GameCode win32_loadGameCode(win32_GameCode gameCode) {
	BOOL res;
	WIN32_FILE_ATTRIBUTE_DATA fileInfo;
	res = GetFileAttributesExA("../ProjectG.dll", GetFileExInfoStandard, &fileInfo);
	assert(res && "Failed to get DLL attribs!");

	if (CompareFileTime(&fileInfo.ftLastWriteTime, &gameCode.dllFileTime) != 0) {
		win32_GameCode newGameCode = {};
		newGameCode.dllFileTime = fileInfo.ftLastWriteTime;
		newGameCode.updateAndRender = win32_guarStub;

		// NOTE(oliver): Need to unload existing DLL to unlock it
		if (gameCode.gameCodeDLL) {
			res = FreeLibrary(gameCode.gameCodeDLL);
			assert(res && "Failed to unload DLL!");
		}
		res = CopyFileA("../ProjectG.dll", "../ProjectG_tmp.dll", false /* overwrite */);
		assert(res && "Failed to copy DLL!");

		newGameCode.gameCodeDLL = LoadLibraryA("../ProjectG_tmp.dll");
		if (newGameCode.gameCodeDLL) {
			newGameCode.updateAndRender = (f_gameUpdateAndRender*)GetProcAddress(
				newGameCode.gameCodeDLL,
				"gameUpdateAndRender");

			if (!newGameCode.updateAndRender) {
				newGameCode.updateAndRender = win32_guarStub;
			}
		}

		return newGameCode;
	}

	return gameCode;
}

internal_func Handles win32_createHandles() {
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
	win32_GameCode game{};
	game = win32_loadGameCode(game);
	Handles handles = win32_createHandles();
	GameMemory memory = win32_createMemory();

	if (!ttfInit()) {
		exit(-1);
	}

	renderer.init();
	while (renderer.render()) {
		game = win32_loadGameCode(game);
		game.updateAndRender(&handles, &memory);
		renderer.processCommands();
	}
	renderer.cleanup();

	win32_destroyMemory(&memory);
	return 0;
}
