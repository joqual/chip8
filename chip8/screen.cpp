#define SDL_MAIN_USE_CALLBACKS 1  // use the SDL3 callbacks instead of main
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "chip8.hpp"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static int toggle = 1;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_SetAppMetadata("CHIP-8 Emulator", "1.0", "com.joqual");

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer("CHIP-8 Emulator Screen", 640, 320,
		SDL_WINDOW_RESIZABLE, &window, &renderer))
	{
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	SDL_SetRenderLogicalPresentation(renderer, 640, 480,
		SDL_LOGICAL_PRESENTATION_LETTERBOX);

	// Init chip8
	Chip8* emu = new Chip8();
	*appstate = emu;

	// load program
	emu->load_ROM("c8_test.c8");
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
	emu->cycle();
	SDL_SetRenderDrawColor(renderer, 1, 1, 1, 255);
	SDL_RenderClear(renderer);

	float PIX_WIDTH{ 10.0 };
	float PIX_HEIGHT{ 10.0 };

	for (int row = 0; row < 64; ++row)
	{
		for (int col = 0; col < 32; ++col)
		{
			float f_row = static_cast<float>(row);
			float f_col = static_cast<float>(col);
			SDL_FRect rect = { f_row * PIX_WIDTH, f_col * PIX_HEIGHT, PIX_WIDTH,
							  PIX_HEIGHT };
			int r_value = static_cast<int>(f_row / 64.0 * 255.0);
			int b_value = static_cast<int>(f_col / 32.0 * 255.0);
			SDL_SetRenderDrawColor(renderer, r_value, 0, b_value,
				255);
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	SDL_RenderPresent(renderer);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	delete appstate;
}