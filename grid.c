#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <err.h>

void fix_contrast(int r, int g, int b, int min_int, int max_int)
{
    r = ;
    if(r > 255)
        r =255;
}

void surface_to_no_green(SDL_Surface *surface)
{
    Uint32 *pix = (Uint32 *)surface->pixels;
    SDL_LockSurface(surface);
    SDL_PixelFormat *format = surface->format;
    int min_int = 255;
    int max_int = 0;
    int intencity = 0;
    for(int i = 0; i<surface->w; i++)
    {
        for(int j =0; j<surface->h;j++)
        {
            Uint32 p = pix[ i + (j * surface->w)];
            Uint8 r = 0;
            Uint8 g = 0;
            Uint8 b = 0;
            SDL_GetRGB(p,format,&r,&g,&b);
            r = (r*)/100;
            g = (g*)/100;
            b = (b*)/100;
            intencity = (r+g+b)/3;
            if(intencity > max_int)
                max_int = intencity;
            if (intencity < min_int)
                min_int = intencity;
            p = SDL_MapRGB(format,r,g,b);
        }
    }
    for(int i = 0; i<surface->w; i++)
    {
        for(int j =0; j<surface->h;j++)
        {
            Uint32 p = pix[ i + (j * surface->w)];
            Uint8 r = 0;
            Uint8 g = 0;
            Uint8 b = 0;
            SDL_GetRGB(p,format,&r,&g,&b)
            fix_contrast(r,g,b,min_int,max_int)
        }
    }
    SDL_UnlockSurface(surface);
}

int main(int argc, char** argv)
{
    if (argc != 2)
       errx(EXIT_FAILURE, "Usage: image-file");
    if(SDL_Init(SDL_INIT_VIDEO)!=0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow("Surface_to_no_green", 0, 0, 0, 0,
		    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* t = IMG_Load(argv[1]);
    if (t == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Surface *surface = SDL_ConvertSurfaceFormat(t, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(t);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_SetWindowSize(window,  surface->w, surface->h);

    //
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);
    SDL_Event event;
    while (1)
    {
        SDL_WaitEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT:
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return EXIT_SUCCESS;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                        SDL_RenderCopy(renderer,texture,NULL,NULL);
                        SDL_RenderPresent(renderer);
                }
                break;
        }
    }
    return EXIT_SUCCESS;
}
