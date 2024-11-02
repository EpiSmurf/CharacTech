#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <err.h>

// Adjusts the contrast of a pixel's RGB values based on minimum and maximum intensities
void fix_contrast(Uint8 *r, Uint8 *g, Uint8 *b, int min_int, int max_int)
{
    int temp = 0;
    
    // Adjust red channel
    temp = ((*r - min_int) * (255 / (max_int - min_int)));
    if (temp > 255) temp = 255;
    if (temp < 0) temp = 0;
    *r = temp;

    // Adjust green channel
    temp = ((*g - min_int) * (255 / (max_int - min_int)));
    if (temp > 255) temp = 255;
    if (temp < 0) temp = 0;
    *g = temp;

    // Adjust blue channel
    temp = ((*b - min_int) * (255 / (max_int - min_int)));
    if (temp > 255) temp = 255;
    if (temp < 0) temp = 0;
    *b = temp;
}

// Converts an SDL surface to grayscale
void grayscale(SDL_Surface *surface)
{
    Uint32 *pix = (Uint32 *)surface->pixels;
    SDL_LockSurface(surface);
    SDL_PixelFormat *format = surface->format;

    int min_int = 255;
    int max_int = 0;
    int intensity = 0;
    int gray = 0;

    // Convert each pixel to grayscale and calculate intensity range
    for (int i = 0; i < surface->w; i++)
    {
        for (int j = 0; j < surface->h; j++)
        {
            Uint32 p = pix[i + (j * surface->w)];
            Uint8 r = 0, g = 0, b = 0;

            SDL_GetRGB(p, format, &r, &g, &b);
            r = (r * 30) / 100;
            g = (g * 59) / 100;
            b = (b * 11) / 100;
            gray = r + g + b;

            intensity = (r + g + b) / 3;
            if (intensity > max_int) max_int = intensity;
            if (intensity < min_int) min_int = intensity;

            p = SDL_MapRGB(format, gray, gray, gray);
            pix[i + (j * surface->w)] = p;
        }
    }

    // Uncomment this section to adjust contrast after grayscale conversion
    /*
    for (int i = 0; i < surface->w; i++)
    {
        for (int j = 0; j < surface->h; j++)
        {
            Uint32 p = pix[i + (j * surface->w)];
            Uint8 r = 0, g = 0, b = 0;

            SDL_GetRGB(p, format, &r, &g, &b);
            fix_contrast(&r, &g, &b, min_int, max_int);
            p = SDL_MapRGB(format, r, g, b);
            pix[i + (j * surface->w)] = p;
        }
    }
    */

    SDL_UnlockSurface(surface);
}

int main(int argc, char** argv)
{
    printf("Image to load: '%s'\n", argv[1]);

    // Initialize SDL and SDL_image
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "SDL initialization error: %s", SDL_GetError());
    if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG))
        errx(EXIT_FAILURE, "Failed to initialize SDL_image: %s", SDL_GetError());

    // Check for correct argument count
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: %s <image-file>", argv[0]);

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window)
        errx(EXIT_FAILURE, "Window creation error: %s", SDL_GetError());

    // Create SDL renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
        errx(EXIT_FAILURE, "Renderer creation error: %s", SDL_GetError());

    // Load image file into a surface
    SDL_Surface* t = IMG_Load(argv[1]);
    if (!t)
        errx(EXIT_FAILURE, "Image load error: %s", SDL_GetError());

    // Convert surface to required format
    SDL_Surface *surface = SDL_ConvertSurfaceFormat(t, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(t);
    if (!surface)
        errx(EXIT_FAILURE, "Surface conversion error: %s", SDL_GetError());

    SDL_SetWindowSize(window, surface->w, surface->h);

    // Apply grayscale filter to the surface
    grayscale(surface);

    // Create texture from the modified surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Render texture to window
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (1)
    {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT)
        {
            // Cleanup and exit on quit event
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            IMG_Quit();
            SDL_Quit();
            return EXIT_SUCCESS;
        }
        else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            // Re-render on window resize
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    return EXIT_SUCCESS;
}
