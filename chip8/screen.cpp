#define SDL_MAIN_USE_CALLBACKS 1  // use the SDL3 callbacks instead of main
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "chip8.hpp"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_SetAppMetadata("CHIP-8 Emulator", "1.0", "com.joqual");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("CHIP-8 Emulator Screen", 450, 450, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    int WIDTH = 50;
    int HEIGHT = 50;
    int red = 1;
    int blue = 0;
    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 10; ++col) {
            SDL_FRect rect = { col * WIDTH, row * HEIGHT, WIDTH, HEIGHT };
            red ^= 1;
            blue ^= 1;
            SDL_SetRenderDrawColor(renderer, row * 25, 0, col * 25, 255);

            // Copy the specific tile from the spritesheet to the screen
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}