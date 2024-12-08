#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Fonction pour ajuster le contraste
Uint32 adjust_contrast(Uint32 pixel, float contrast, SDL_PixelFormat *format) {
    Uint8 r, g, b;
    SDL_GetRGB(pixel, format, &r, &g, &b);

    // Ajustement du contraste
    float factor = (259 * (contrast + 255)) / (255 * (259 - contrast));
    r = SDL_clamp(factor * (r - 128) + 128, 0, 255);
    g = SDL_clamp(factor * (g - 128) + 128, 0, 255);
    b = SDL_clamp(factor * (b - 128) + 128, 0, 255);

    return SDL_MapRGB(format, r, g, b);
}

// Fonction pour appliquer un filtre aux pixels isolés
void filter_isolated_pixels(SDL_Surface *surface) {
    int width = surface->w;
    int height = surface->h;
    Uint32 *pixels = (Uint32 *)surface->pixels;

    // Créer une copie des pixels pour le traitement
    Uint32 *temp_pixels = malloc(width * height * sizeof(Uint32));
    if (!temp_pixels) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return;
    }
    memcpy(temp_pixels, pixels, width * height * sizeof(Uint32));

    // Parcourir les pixels
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int index = y * width + x;
            Uint32 current = temp_pixels[index];
            //int similar_count = 0;

            // Vérifier les 8 voisins
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx != 0 && dy != 0){
                        int neighbor_index = (y + dy) * width + (x + dx);
                        if (temp_pixels[neighbor_index] == current) {
                            //similar_count++;
                        }
                    }
                }
            }

            // Si moins de 2 voisins similaires, rendre le pixel noir
           // if (similar_count < 2) {
           //     pixels[index] = SDL_MapRGB(format, 0, 0, 0);
           // }
        }
    }

    free(temp_pixels);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <image.png>\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (!IMG_Init(IMG_INIT_PNG)) {
        fprintf(stderr, "Erreur SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface *image = IMG_Load(argv[1]);
    if (!image) {
        fprintf(stderr, "Erreur chargement image: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Filtrer les pixels isolés
    filter_isolated_pixels(image);

    // Ajuster le contraste
    float contrast = 100.0f; // Valeure a ajuster pour changer le contraste
    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            int index = y * image->w + x;
            Uint32 pixel = ((Uint32 *)image->pixels)[index];
            ((Uint32 *)image->pixels)[index] = adjust_contrast(pixel, contrast, image->format);
        }
    }

    // Générer le nom du fichier de sortie
    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%s", argv[1]);
    char *dot = strrchr(output_filename, '.');
    if (dot) {
        strcpy(dot, "_filtered.png");
    }

    // Sauvegarder l'image
    if (IMG_SavePNG(image, output_filename) != 0) {
        fprintf(stderr, "Erreur sauvegarde image: %s\n", IMG_GetError());
    } else {
        printf("Image sauvegardée sous le nom : %s\n", output_filename);
    }

    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
