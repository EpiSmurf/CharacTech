#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

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

    // Load image into a surface
    SDL_Surface* temp_surface = IMG_Load(argv[1]);
    if (!temp_surface)
        errx(EXIT_FAILURE, "Image loading error: %s", IMG_GetError());

    // Convert surface to target format
    SDL_Surface* surface = SDL_ConvertSurfaceFormat(temp_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(temp_surface);
    if (!surface)
        errx(EXIT_FAILURE, "Surface conversion error: %s", SDL_GetError());

    // Parse rotation angle from arguments
    char* end;
    double angle = strtod(argv[2], &end);
    if (*end != '\0')
        errx(EXIT_FAILURE, "Invalid angle: %s", argv[2]);

    // Create a new surface to render the rotated image
    SDL_Surface* rotated_surface = SDL_CreateRGBSurfaceWithFormat(0, surface->w, surface->h, 32, SDL_PIXELFORMAT_RGBA8888);
    if (!rotated_surface)
        errx(EXIT_FAILURE, "Rotated surface creation error: %s", SDL_GetError());

    // Create an SDL renderer with the new surface as target
    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(rotated_surface);
    if (!renderer)
        errx(EXIT_FAILURE, "Renderer creation error: %s", SDL_GetError());

    // Create a texture from the original surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);  // Free the original surface as we now have a texture
    if (!texture)
        errx(EXIT_FAILURE, "Texture creation error: %s", SDL_GetError());

    // Set the rendering target to the rotated surface
    SDL_SetRenderTarget(renderer, NULL);

    // Set rotation pivot to the center of the image
    SDL_Point pivot = { rotated_surface->w / 2, rotated_surface->h / 2 };
    SDL_Rect dest_rect = { 0, 0, rotated_surface->w, rotated_surface->h };

    // Clear and render the rotated image
    SDL_RenderClear(renderer);
    SDL_RenderCopyEx(renderer, texture, NULL, &dest_rect, angle, &pivot, SDL_FLIP_NONE);
    SDL_RenderPresent(renderer);

    // Create output file path with "_turned" suffix
    char output_path[256];
    snprintf(output_path, sizeof(output_path), "%s_turned.png", strtok(argv[1], "."));

    // Save the rotated surface as a PNG file
    if (IMG_SavePNG(rotated_surface, output_path) != 0)
        errx(EXIT_FAILURE, "Failed to save image: %s", IMG_GetError());

    printf("Saved rotated image as %s\n", output_path);

    // Cleanup resources
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(rotated_surface);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}