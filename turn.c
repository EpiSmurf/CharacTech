#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <err.h>

int main(int argc, char** argv)
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    if(!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG))
        errx(EXIT_FAILURE, "Failed to initialise SDL_image: %s", SDL_GetError());


    if (argc != 3)
       errx(EXIT_FAILURE, "Wrong amount of arguments");
    if(SDL_Init(SDL_INIT_VIDEO)!=0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow("Window",0, 0, 0, 0,
		    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* t = IMG_Load(argv[1]);
    if (t == NULL)
        {
            errx(EXIT_FAILURE, "%s", SDL_GetError());
        }
    SDL_Surface *surface = SDL_ConvertSurfaceFormat(t, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(t);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_SetWindowSize(window,  surface->w, surface->h);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer,texture,NULL,NULL);

    float Rad = SDL_deg2rad(argv[2]);
    SDL_Rect DRect = {0,0,surface->w,surface->h};
    SDL_RenderCopyEx(renderer,texture,NULL,DRect,Rad,{surface->w/2,surface->/2},SDL_FLIP_NONE);

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
