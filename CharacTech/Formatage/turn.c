#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char** argv)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "SDL initialization error: %s", SDL_GetError());

    // Initialize SDL_image with JPG support
    if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG))
        errx(EXIT_FAILURE, "Failed to initialize SDL_image with JPG support: %s", IMG_GetError());

    // Check for correct argument count
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: %s <image_path> <angle>", argv[0]);

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window)
        errx(EXIT_FAILURE, "Window creation error: %s", SDL_GetError());

    // Create SDL renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
        errx(EXIT_FAILURE, "Renderer creation error: %s", SDL_GetError());

    // Load image into a surface
    SDL_Surface* temp_surface = IMG_Load(argv[1]);
    if (!temp_surface)
        errx(EXIT_FAILURE, "Image loading error: %s", IMG_GetError());

    // Convert surface to target format
    SDL_Surface* surface = SDL_ConvertSurfaceFormat(temp_surface, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(temp_surface);
    if (!surface)
        errx(EXIT_FAILURE, "Surface conversion error: %s", SDL_GetError());

    // Set window size to match the image
    SDL_SetWindowSize(window, surface->w, surface->h);

    // Create a texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);  // Free surface after texture creation
    if (!texture)
        errx(EXIT_FAILURE, "Texture creation error: %s", SDL_GetError());

    // Parse rotation angle from arguments
    char* end;
    double angle = strtod(argv[2], &end);
    if (*end != '\0')
        errx(EXIT_FAILURE, "Invalid angle: %s", argv[2]);

    // Set rotation pivot to the center of the image
    SDL_Point pivot = { surface->w / 2, surface->h / 2 };
    SDL_Rect dest_rect = { 0, 0, surface->w, surface->h };

    // Render the image with rotation
    SDL_RenderClear(renderer);
    SDL_RenderCopyEx(renderer, texture, NULL, &dest_rect, angle, &pivot, SDL_FLIP_NONE);
    SDL_RenderPresent(renderer);

    // Main event loop
    SDL_Event event;
    int running = 1;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
            else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                // Re-render image on resize
                SDL_RenderClear(renderer);
                SDL_RenderCopyEx(renderer, texture, NULL, &dest_rect, angle, &pivot, SDL_FLIP_NONE);
                SDL_RenderPresent(renderer);
            }
        }
    }

    // Cleanup resources
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
