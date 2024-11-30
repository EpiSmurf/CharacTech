#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <math.h>

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

    // Calculate the new dimensions of the rotated image
    double radians = angle * M_PI / 180.0;
    int new_w = (int)(surface->w * fabs(cos(radians)) + surface->h * fabs(sin(radians)));
    int new_h = (int)(surface->w * fabs(sin(radians)) + surface->h * fabs(cos(radians)));

    // Create a new surface to render the rotated image (with new dimensions)
    SDL_Surface* rotated_surface = SDL_CreateRGBSurfaceWithFormat(0, new_w, new_h, 32, SDL_PIXELFORMAT_RGBA8888);
    if (!rotated_surface)
        errx(EXIT_FAILURE, "Rotated surface creation error: %s", SDL_GetError());

    // Create an SDL window
    SDL_Window* window = SDL_CreateWindow("Image Rotation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, surface->w, surface->h, SDL_WINDOW_SHOWN);
    if (!window)
        errx(EXIT_FAILURE, "Window creation error: %s", SDL_GetError());

    // Create an SDL renderer (use hardware renderer here)
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
        errx(EXIT_FAILURE, "Renderer creation error: %s", SDL_GetError());

    // Create a texture from the original surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);  // Free the original surface as we now have a texture
    if (!texture)
        errx(EXIT_FAILURE, "Texture creation error: %s", SDL_GetError());

    // Set the rendering target to the rotated surface
    SDL_SetRenderTarget(renderer, rotated_surface);

    // Set rotation pivot to the center of the new rotated surface
    SDL_Point pivot = { new_w / 2, new_h / 2 };

    // Set the destination rectangle (we want to keep the image centered)
    SDL_Rect dest_rect = { (rotated_surface->w - new_w) / 2, (rotated_surface->h - new_h) / 2, new_w, new_h };

    // Clear and render the rotated image
    SDL_RenderClear(renderer);
    SDL_RenderCopyEx(renderer, texture, NULL, &dest_rect, angle, &pivot, SDL_FLIP_NONE);
    SDL_RenderPresent(renderer);

    // Create output file path with "_turned" suffix
    char output_path[256];
    char* dot = strrchr(argv[1], '.');
    if (dot)
        *dot = '\0';  // Null-terminate the filename at the dot
    snprintf(output_path, sizeof(output_path), "%s_turned.png", argv[1]);

    // Save the rotated surface as a PNG file
    if (IMG_SavePNG(rotated_surface, output_path) != 0)
        errx(EXIT_FAILURE, "Failed to save image: %s", IMG_GetError());

    printf("Saved rotated image as %s\n", output_path);

    // Cleanup resources
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(rotated_surface);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}