#pragma once

#ifdef _WIN32
# define EXPORT __declspec( dllexport )
#else
# define EXPORT
#endif

// NOTE(oliver): engine handles
struct Handles {
	void (*DEBUG_log)(const char *);
};

#define GAME_UPDATE_AND_RENDER(name) int name(Handles)
typedef GAME_UPDATE_AND_RENDER(f_gameUpdateAndRender);
GAME_UPDATE_AND_RENDER(gameUpdateAndRenderStub) {
	return(0);
}
