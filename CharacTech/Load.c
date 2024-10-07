#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define WHITE_THRESHOLD 200

// Function to determine if a color is considered "white"
bool is_white(SDL_Color color) {
    // Check if the RGB values are above the defined threshold
    return color.r >= WHITE_THRESHOLD && color.g >= WHITE_THRESHOLD && color.b >= WHITE_THRESHOLD;
}

// Function to detect the bounding box of the grid in the image
SDL_Rect detect_grid(SDL_Surface *surface) {
    int width = surface->w;   // Width of the image
    int height = surface->h;  // Height of the image
    int left = width, right = 0, top = height, bottom = 0;
    
    // Traverse the image pixel by pixel to find the borders of the grid
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Get the pixel value at (x, y)
            Uint32 pixel = ((Uint32 *)surface->pixels)[y * width + x];
            SDL_Color color;
            // Extract the RGB values from the pixel
            SDL_GetRGB(pixel, surface->format, &color.r, &color.g, &color.b);
            
            // If the pixel is not white, update the bounding box
            if (!is_white(color)) {
                if (x < left) left = x;     // Update left boundary
                if (x > right) right = x;   // Update right boundary
                if (y < top) top = y;       // Update top boundary
                if (y > bottom) bottom = y; // Update bottom boundary
            }
        }
    }
    
    // Create a rectangle representing the bounding box of the grid
    SDL_Rect grid_rect = {left, top, right - left + 1, bottom - top + 1};
    return grid_rect;
}

// Function to crop the image to the specified rectangle
SDL_Surface* crop_image(SDL_Surface *surface, SDL_Rect crop_rect) {
    // Create a new surface to hold the cropped image
    SDL_Surface *cropped = SDL_CreateRGBSurface(0, crop_rect.w, crop_rect.h, surface->format->BitsPerPixel,
                                                surface->format->Rmask, surface->format->Gmask,
                                                surface->format->Bmask, surface->format->Amask);
    if (cropped == NULL) {
        // Handle error if the cropped surface could not be created
        fprintf(stderr, "SDL_CreateRGBSurface failed: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Copy the specified rectangle from the original surface to the new cropped surface
    SDL_BlitSurface(surface, &crop_rect, cropped, NULL);
    return cropped;
}

int main(int argc, char *argv[]) {
    // Ensure that the correct number of arguments are provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <image_file>\n", argv[0]);
        return 1;
    }
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    
    // Initialize SDL_image for PNG and JPG support
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    
    // Load the image specified by the user
    SDL_Surface *image = IMG_Load(argv[1]);
    if (!image) {
        // Handle error if the image could not be loaded
        fprintf(stderr, "IMG_Load Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Detect the grid in the loaded image
    SDL_Rect grid_rect = detect_grid(image);
    
    // Crop the image to the detected grid
    SDL_Surface *cropped_image = crop_image(image, grid_rect);
    if (cropped_image == NULL) {
        // Handle error if cropping failed
        SDL_FreeSurface(image);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    // Save the cropped image to a new file
    IMG_SavePNG(cropped_image, "cropped_image.png");
    
    // Free the surfaces and quit SDL
    SDL_FreeSurface(image);
    SDL_FreeSurface(cropped_image);
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}