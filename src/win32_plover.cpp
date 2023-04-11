// PLOVER WIN32 PLATFORM LAYER
// All win32 code should be in here

#include "plover/Renderer.h"
#include "plover/plover.h"

#include <windows.h>

using namespace Plover;

struct Win32_GameCode
{
	f_gameUpdateAndRender* updateAndRender;
};

static Win32_GameCode win32_LoadGameCode() {
	Win32_GameCode result = {};
	result.updateAndRender = gameUpdateAndRenderStub;

	HMODULE gameCodeDLL = LoadLibraryA("../ProjectG.dll");
	if (gameCodeDLL) {
		result.updateAndRender = (f_gameUpdateAndRender*)GetProcAddress(gameCodeDLL, "gameUpdateAndRender");
		if (!result.updateAndRender) result.updateAndRender = gameUpdateAndRenderStub;
	}

	return result;
}

static void win32_DEBUG_log(const char *str) {
	OutputDebugStringA(str);
}

static Handles win32_CreateHandles() {
	Handles handles{};
	handles.DEBUG_log = win32_DEBUG_log;
	return handles;
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
) {
	Win32_GameCode game = win32_LoadGameCode();
	Handles handles = win32_CreateHandles();
	Renderer renderer{};

	renderer.init();
	while (renderer.render()) {
		game.updateAndRender(handles);
	}
	renderer.cleanup();

	return 0;
}
