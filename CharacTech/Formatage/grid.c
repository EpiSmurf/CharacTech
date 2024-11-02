#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

// Fonction pour détecter automatiquement les positions des cellules dans la grille
void detect_grid(SDL_Surface *surface, int *cell_width, int *cell_height, int *grid_start_x, int *grid_start_y, int *grid_cols, int *grid_rows)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    SDL_PixelFormat *format = surface->format;
    int width = surface->w;
    int height = surface->h;

    int last_x = -1, last_y = -1;
    *grid_start_x = width, *grid_start_y = height;

    // Détection des colonnes (limite en largeur pour éviter une colonne en trop)
    for (int x = 1; x < width - 1; x++) {
        for (int y = 0; y < height / 2; y++) {  // Limiter la recherche à la moitié supérieure
            Uint8 r, g, b;
            SDL_GetRGB(pixels[y * width + x], format, &r, &g, &b);
            int intensity = (r + g + b) / 3;

            if (intensity < 128) { // Si c'est sombre, détecter le bord
                if (last_x == -1 || x - last_x > 1) {
                    (*grid_cols)++;
                    *cell_width = (last_x == -1) ? x : (x - *grid_start_x) / (*grid_cols - 1);
                    if (x < *grid_start_x) *grid_start_x = x;
                }
                last_x = x;
                break;
            }
        }
    }
    (*grid_cols)--; // Réduire d'une colonne pour corriger la détection trop large

    // Détection des lignes, en continuant jusqu'à la fin
    for (int y = 1; y < height; y++) {
        int row_detected = 0;
        for (int x = 0; x < width / 2; x++) { // Limiter la recherche à la moitié gauche de l'image
            Uint8 r, g, b;
            SDL_GetRGB(pixels[y * width + x], format, &r, &g, &b);
            int intensity = (r + g + b) / 3;

            if (intensity < 128) { // Si c'est sombre, détecter le bord
                if (last_y == -1 || y - last_y > 1) {
                    (*grid_rows)++;
                    *cell_height = (last_y == -1) ? y : (y - *grid_start_y) / (*grid_rows - 1);
                    if (y < *grid_start_y) *grid_start_y = y;
                }
                last_y = y;
                row_detected = 1;
                break;
            }
        }
        
        // Si aucune nouvelle ligne n'est détectée pendant un certain nombre de pixels, considérer que la grille est terminée
        if (!row_detected && (*grid_rows > 0) && (y - last_y) > *cell_height) {
            break;
        }
    }
}

// Applique un filtre de gris sur une surface SDL
void grayscale(SDL_Surface *surface)
{
    Uint32 *pix = (Uint32 *)surface->pixels;
    SDL_LockSurface(surface);
    SDL_PixelFormat *format = surface->format;

    for (int i = 0; i < surface->w; i++) {
        for (int j = 0; j < surface->h; j++) {
            Uint32 p = pix[i + (j * surface->w)];
            Uint8 r = 0, g = 0, b = 0;

            SDL_GetRGB(p, format, &r, &g, &b);
            Uint8 gray = (Uint8)((r * 30 + g * 59 + b * 11) / 100);

            p = SDL_MapRGB(format, gray, gray, gray);
            pix[i + (j * surface->w)] = p;
        }
    }

    SDL_UnlockSurface(surface);
}

// Découpe l'image en lettres en utilisant les paramètres de la grille détectée
void extract_letters(SDL_Surface *surface, int cell_width, int cell_height, int grid_start_x, int grid_start_y, int grid_cols, int grid_rows)
{
    for (int row = 0; row < grid_rows; row++) {
        for (int col = 0; col < grid_cols; col++) {
            SDL_Rect letter_rect = {
                .x = grid_start_x + col * cell_width,
                .y = grid_start_y + row * cell_height,
                .w = cell_width,
                .h = cell_height
            };

            SDL_Surface *letter_surface = SDL_CreateRGBSurface(0, cell_width, cell_height, surface->format->BitsPerPixel,
                                                               surface->format->Rmask, surface->format->Gmask,
                                                               surface->format->Bmask, surface->format->Amask);
            if (!letter_surface) {
                errx(EXIT_FAILURE, "Erreur de création de surface pour lettre : %s", SDL_GetError());
            }

            SDL_BlitSurface(surface, &letter_rect, letter_surface, NULL);

            char filename[64];
            snprintf(filename, sizeof(filename), "letter_%d_%d.png", row, col);
            if (IMG_SavePNG(letter_surface, filename) != 0) {
                errx(EXIT_FAILURE, "Erreur de sauvegarde de l'image %s : %s", filename, SDL_GetError());
            }

            SDL_FreeSurface(letter_surface);
        }
    }
}

int main(int argc, char** argv)
{
    printf("Image à charger : '%s'\n", argv[1]);

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "Erreur d'initialisation de SDL: %s", SDL_GetError());
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
        errx(EXIT_FAILURE, "Erreur d'initialisation de SDL_image: %s", SDL_GetError());

    if (argc != 2)
        errx(EXIT_FAILURE, "Utilisation : %s <fichier-image>", argv[0]);

    SDL_Surface* t = IMG_Load(argv[1]);
    if (!t)
        errx(EXIT_FAILURE, "Erreur de chargement de l'image: %s", SDL_GetError());

    SDL_Surface *surface = SDL_ConvertSurfaceFormat(t, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(t);
    if (!surface)
        errx(EXIT_FAILURE, "Erreur de conversion de surface: %s", SDL_GetError());

    grayscale(surface);

    int cell_width = 0, cell_height = 0;
    int grid_start_x = 0, grid_start_y = 0;
    int grid_cols = 0, grid_rows = 0;
    
    detect_grid(surface, &cell_width, &cell_height, &grid_start_x, &grid_start_y, &grid_cols, &grid_rows);

    extract_letters(surface, cell_width, cell_height, grid_start_x, grid_start_y, grid_cols, grid_rows);

    SDL_FreeSurface(surface);
    IMG_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
