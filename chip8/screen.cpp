#define SDL_MAIN_USE_CALLBACKS 1  // use the SDL3 callbacks instead of main
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "chip8.hpp"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
float PIX_WIDTH{ 10.0 };
float PIX_HEIGHT{ 10.0 };

int WINDOW_WIDTH = Chip8::VIDEO_WIDTH * static_cast<int>(PIX_WIDTH);
int WINDOW_HEIGHT = Chip8::VIDEO_HEIGHT * static_cast<int>(PIX_HEIGHT);

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_SetAppMetadata("CHIP-8 Emulator", "1.0", "com.joqual");

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer("CHIP-8 Emulator Screen", WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_RESIZABLE, &window, &renderer))
	{
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	SDL_SetRenderLogicalPresentation(renderer,
		Chip8::VIDEO_WIDTH,
		Chip8::VIDEO_HEIGHT,
		SDL_LOGICAL_PRESENTATION_LETTERBOX);

	// Init chip8
	Chip8* emu = new Chip8();
	*appstate = emu;

	// load program
	emu->load_ROM("1-chip8-logo.ch8");
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
	Chip8* emu{ static_cast<Chip8*>(appstate) };
	// Run multiple instructions per frame (e.g., 10 cycles @ 60fps = 600Hz)
	for (int i = 0; i < 10; ++i)
	{
		emu->cycle();
	}

	// Update timers at 60Hz (Chip-8 spec)
	emu->update_timers();

	SDL_SetRenderDrawColor(renderer, 1, 1, 1, 255);
	SDL_RenderClear(renderer);

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

			int opacity = emu->video[row][col] == Chip8::PIXEL_ON ? 255 : 0;
			SDL_SetRenderDrawColor(renderer, opacity, 0, opacity, opacity);
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	SDL_RenderPresent(renderer);
	//std::this_thread::sleep_for(std::chrono::milliseconds(500));
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	delete appstate;
}