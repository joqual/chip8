#define SDL_MAIN_USE_CALLBACKS 1  // use the SDL3 callbacks instead of main
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "chip8.hpp"

struct AppState
{
	// SDL will handle window and renderer
	SDL_Window* window{ nullptr };
	SDL_Renderer* renderer{ nullptr };
	std::unique_ptr<Chip8> emu;

	~AppState()
	{
		// SDL3 specific cleanup
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
	}
};

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_SetAppMetadata("CHIP-8 Emulator", "1.0", "com.joqual");

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		return SDL_APP_FAILURE;
	}

	auto state = std::make_unique<AppState>();

	if (!SDL_CreateWindowAndRenderer("CHIP-8 Emulator Screen", Chip8::VIDEO_WIDTH, Chip8::VIDEO_HEIGHT,
		SDL_WINDOW_RESIZABLE, &state->window, &state->renderer))
	{
		return SDL_APP_FAILURE;
	}

	SDL_SetRenderLogicalPresentation(state->renderer,
		Chip8::VIDEO_WIDTH,
		Chip8::VIDEO_HEIGHT,
		SDL_LOGICAL_PRESENTATION_LETTERBOX);

	// Init chip8
	state->emu = std::make_unique<Chip8>();

	// load program
	state->emu->load_ROM("3-corax+.ch8");

	// Transfer ownership of app state to SDL
	*appstate = state.release();
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	if (event->type == SDL_EVENT_QUIT)
	{
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
	AppState* state{ static_cast<AppState*>(appstate) };
	// Run multiple instructions per frame (e.g., 10 cycles @ 60fps = 600Hz)
	for (int i = 0; i < 10; ++i)
	{
		state->emu->cycle();
	}

	// Update timers at 60Hz (Chip-8 spec)
	state->emu->update_timers();

	SDL_SetRenderDrawColor(state->renderer, 1, 1, 1, 255);
	SDL_RenderClear(state->renderer);

	for (int row = 0; row < Chip8::VIDEO_HEIGHT; ++row)
	{
		for (int col = 0; col < Chip8::VIDEO_WIDTH; ++col)
		{
			SDL_FRect rect = {
				static_cast<float>(col),
				static_cast<float>(row),
				1.0f,
				1.0f
			};

			int opacity = state->emu->video[row][col] == Chip8::PIXEL_ON ? 255 : 0;
			SDL_SetRenderDrawColor(state->renderer, opacity, 0, opacity, opacity);
			SDL_RenderFillRect(state->renderer, &rect);
		}
	}

	SDL_RenderPresent(state->renderer);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	if (appstate)
	{
		// Reclaim ownership of app state, let it go out of scope to free memory
		std::unique_ptr<AppState> cleanup{ static_cast<AppState*>(appstate) };
	}
}
