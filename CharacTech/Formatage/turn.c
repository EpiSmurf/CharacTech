#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// -----------------------------------------
// Fonctions utilitaires internes
// -----------------------------------------

// Calcule l'histogramme d'une image 8 bits
void compute_histogram(Uint8* pixels, int size, int histogram[256]) {
    for (int i=0; i<256; i++)
        histogram[i]=0;
    for (int i=0; i<size; i++){
        histogram[pixels[i]]++;
    }
}

// Seuillage Otsu
Uint8 otsu_threshold(Uint8* pixels, int size) {
    int histogram[256];
    compute_histogram(pixels,size,histogram);
    int total=size;
    double sum=0;
    for(int i=0;i<256;i++){
        sum+=i*(double)histogram[i];
    }

    double sumB=0;
    int wB=0;
    int wF=0;
    double varMax=0;
    int threshold=0;
    for (int i=0; i<256;i++){
        wB+=histogram[i];
        if (wB==0) continue;
        wF=total-wB;
        if (wF==0) break;
        sumB+= (double)(i*histogram[i]);
        double mB=sumB/wB;
        double mF=(sum - sumB)/wF;
        double varBetween=(double)wB*(double)wF*(mB - mF)*(mB - mF);
        if (varBetween>varMax) {
            varMax=varBetween;
            threshold=i;
        }
    }
    return (Uint8)threshold;
}

// Détection de lignes par transformée de Hough
typedef struct {
    double rho;
    double theta;
} Line;

Line* hough_lines(Uint8* edges, int w, int h, int* line_count) {
    double diag = sqrt((double)w*w+(double)h*h);
    double rho_res=1.0;
    double theta_res=M_PI/180.0; // 1 degré
    int nrho=(int)(2*diag/rho_res);
    int ntheta=(int)(M_PI/theta_res);

    int* acc=calloc(nrho*ntheta,sizeof(int));
    if(!acc) errx(EXIT_FAILURE,"no mem hough acc");
    double half_rho=nrho/2.0;

    // Accumulation
    for (int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            if(edges[y*w+x]==255){
                for(int t=0;t<ntheta;t++){
                    double theta=t*theta_res;
                    double rho_val=x*cos(theta)+y*sin(theta);
                    int r=(int)(rho_val/rho_res+half_rho);
                    if(r>=0 && r<nrho) {
                        acc[r*ntheta+t]++;
                    }
                }
            }
        }
    }

    // Trouver le max pour seuil automatique
    int max_val=0;
    for(int i=0;i<nrho*ntheta;i++){
        if(acc[i]>max_val) max_val=acc[i];
    }

    if(max_val==0) {
        // pas de lignes
        free(acc);
        *line_count=0;
        return NULL;
    }

    // Seuil automatique : par ex. la moitié du max
    int hough_threshold=max_val/2;
    if(hough_threshold<1) hough_threshold=1;

    int max_lines=2000;
    Line* lines=malloc(max_lines*sizeof(Line));
    if(!lines) errx(EXIT_FAILURE,"no mem lines");
    int count=0;
    for(int r=0;r<nrho;r++){
        for(int t=0;t<ntheta;t++){
            if(acc[r*ntheta+t]>=hough_threshold) {
                if(count<max_lines) {
                    double rho=(r - half_rho)*rho_res;
                    double theta=t*theta_res;
                    lines[count].rho=rho;
                    lines[count].theta=theta;
                    count++;
                }
            }
        }
    }

    *line_count=count;
    free(acc);
    return lines;
}

// Calcule l'angle dominant à partir des lignes détectées
double calculate_angle_from_lines(Line* lines, int count) {
    if(count==0) return 0.0;
    int hist[181];
    for(int i=0;i<181;i++)
        hist[i]=0;
    for(int i=0;i<count;i++){
        double deg=lines[i].theta*180.0/M_PI;
        // Normaliser entre -90 et 90
        if(deg>90) deg-=180;
        else if(deg<-90) deg+=180;
        int idx=(int)round(deg+90.0);
        if(idx<0) idx=0;
        if(idx>180) idx=180;
        hist[idx]++;
    }

    int max_count=0;
    int max_idx=90;
    for(int i=0;i<181;i++){
        if(hist[i]>max_count) {
            max_count=hist[i];
            max_idx=i;
        }
    }
    double dominant_angle=(double)max_idx-90.0;
    return -dominant_angle;
}

// -----------------------------------------
// Fonction principale de calcul de l'angle
// -----------------------------------------
double calculate_rotation_angle(SDL_Surface* surface) {
    int width = surface->w;
    int height = surface->h;
    Uint32* pixels = (Uint32*)surface->pixels;

    // Calcul du gradient Sobel
    int gx, gy;
    // On stocke la magnitude du gradient dans un buffer 8 bits
    Uint8* gradient_mag = malloc(width*height);
    if(!gradient_mag) errx(EXIT_FAILURE,"no mem gradient");
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            Uint8 left = ((pixels[y * width + x - 1] & 0xFF) +
                          ((pixels[y * width + x - 1] >> 8) & 0xFF) +
                          ((pixels[y * width + x - 1] >> 16) & 0xFF)) / 3;
            Uint8 right = ((pixels[y * width + x + 1] & 0xFF) +
                           ((pixels[y * width + x + 1] >> 8) & 0xFF) +
                           ((pixels[y * width + x + 1] >> 16) & 0xFF)) / 3;
            Uint8 top = ((pixels[(y - 1) * width + x] & 0xFF) +
                         ((pixels[(y - 1) * width + x] >> 8) & 0xFF) +
                         ((pixels[(y - 1) * width + x] >> 16) & 0xFF)) / 3;
            Uint8 bottom = ((pixels[(y + 1) * width + x] & 0xFF) +
                            ((pixels[(y + 1) * width + x] >> 8) & 0xFF) +
                            ((pixels[(y + 1) * width + x] >> 16) & 0xFF)) / 3;

            gx = right - left;
            gy = bottom - top;
            double mag = sqrt((double)gx*gx+(double)gy*gy);
            if(mag>255) mag=255;
            gradient_mag[y*width+x]=(Uint8)mag;
        }
    }
    // Bords
    for(int x=0;x<width;x++){
        gradient_mag[x]=0;
        gradient_mag[(height-1)*width+x]=0;
    }
    for(int y=0;y<height;y++){
        gradient_mag[y*width]=0;
        gradient_mag[y*width+(width-1)]=0;
    }

    // Seuillage Otsu sur la magnitude pour obtenir une image binaire d'arêtes
    Uint8 th = otsu_threshold(gradient_mag, width*height);
    for(int i=0;i<width*height;i++){
        gradient_mag[i]=(gradient_mag[i]>=th)?255:0;
    }

    // Transformée de Hough
    int line_count=0;
    Line* lines=hough_lines(gradient_mag,width,height,&line_count);
    free(gradient_mag);

    double angle=0.0;
    if(line_count>0) {
        angle=calculate_angle_from_lines(lines,line_count);
        free(lines);
    } else {
        // Pas de lignes, angle = 0
        angle=0.0;
    }

    return angle;
}

// -----------------------------------------
// Code principal inchangé (sauf l'appel à la fonction ci-dessus)
// -----------------------------------------
int main(int argc, char** argv) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "SDL initialization error: %s", SDL_GetError());

    // Initialize SDL_image with JPG and PNG support
    if (!(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) & (IMG_INIT_JPG | IMG_INIT_PNG)))
        errx(EXIT_FAILURE, "Failed to initialize SDL_image: %s", IMG_GetError());

    // Check for correct argument count
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: %s <image_path>", argv[0]);

    // Load image into a surface
    SDL_Surface* temp_surface = IMG_Load(argv[1]);
    if (!temp_surface)
        errx(EXIT_FAILURE, "Image loading error: %s", IMG_GetError());

    // Convert surface to target format
    SDL_Surface* surface = SDL_ConvertSurfaceFormat(temp_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(temp_surface);
    if (!surface)
        errx(EXIT_FAILURE, "Surface conversion error: %s", SDL_GetError());

    // Calculate the rotation angle
    double angle = calculate_rotation_angle(surface);
    printf("Angle de rotation détecté: %.2f degrés\n", angle);

    // Calculate the new dimensions of the rotated image
    double radians = angle * M_PI / 180.0;
    int original_w = surface->w;
    int original_h = surface->h;
    double sin_radians = fabs(sin(radians));
    double cos_radians = fabs(cos(radians));
    int new_w = (int)(original_w * cos_radians + original_h * sin_radians);
    int new_h = (int)(original_w * sin_radians + original_h * cos_radians);


    // Create an SDL window
    SDL_Window* window = SDL_CreateWindow("Image Rotation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, new_w, new_h, SDL_WINDOW_HIDDEN);
    if (!window)
        errx(EXIT_FAILURE, "Window creation error: %s", SDL_GetError());

    // Create an SDL renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    if (!renderer)
        errx(EXIT_FAILURE, "Renderer creation error: %s", SDL_GetError());

    // Create a texture from the original surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture)
        errx(EXIT_FAILURE, "Texture creation error: %s", SDL_GetError());

    // Enable alpha blending
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // Create a render target texture to render the rotated image
    SDL_Texture* target_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, new_w, new_h);
    if (!target_texture)
        errx(EXIT_FAILURE, "Target texture creation error: %s", SDL_GetError());

    // Enable alpha blending on the target texture
    SDL_SetTextureBlendMode(target_texture, SDL_BLENDMODE_BLEND);

    // Set the rendering target to the target texture
    if (SDL_SetRenderTarget(renderer, target_texture) != 0)
        errx(EXIT_FAILURE, "SDL_SetRenderTarget failed: %s", SDL_GetError());

    // Clear the render target with transparent color
    if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0) != 0)
        errx(EXIT_FAILURE, "SDL_SetRenderDrawColor failed: %s", SDL_GetError());
    if (SDL_RenderClear(renderer) != 0)
        errx(EXIT_FAILURE, "SDL_RenderClear failed: %s", SDL_GetError());

    // Set rotation pivot to the center of the original image
    SDL_Point pivot = { original_w / 2, original_h / 2 };

    // Calculate the position to center the rotated image
    SDL_Rect dest_rect = { (new_w - original_w) / 2, (new_h - original_h) / 2, original_w, original_h };

    // Render the rotated image onto the target texture
    if (SDL_RenderCopyEx(renderer, texture, NULL, &dest_rect, angle, &pivot, SDL_FLIP_NONE) != 0)
        errx(EXIT_FAILURE, "SDL_RenderCopyEx failed: %s", SDL_GetError());

    // Reset the rendering target to the default (window)
    if (SDL_SetRenderTarget(renderer, NULL) != 0)
        errx(EXIT_FAILURE, "SDL_SetRenderTarget failed: %s", SDL_GetError());

    // Create a surface to read pixels into
    SDL_Surface* rotated_surface = SDL_CreateRGBSurfaceWithFormat(0, new_w, new_h, 32, SDL_PIXELFORMAT_RGBA8888);
    if (!rotated_surface)
        errx(EXIT_FAILURE, "Rotated surface creation error: %s", SDL_GetError());

    // Read pixels from the target texture into the surface
    if (SDL_SetRenderTarget(renderer, target_texture) != 0)
        errx(EXIT_FAILURE, "SDL_SetRenderTarget failed: %s", SDL_GetError());
    if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888, rotated_surface->pixels, rotated_surface->pitch) != 0)
        errx(EXIT_FAILURE, "SDL_RenderReadPixels failed: %s", SDL_GetError());

    // Create output file path with "_turned" suffix
    char output_path[256];
    snprintf(output_path, sizeof(output_path), "%s_turned.png", argv[1]);

    // Save the rotated surface as a PNG file
    if (IMG_SavePNG(rotated_surface, output_path) != 0)
        errx(EXIT_FAILURE, "Failed to save image: %s", IMG_GetError());

    printf("Image pivotée enregistrée sous %s\n", output_path);

    // Cleanup resources
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(target_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(rotated_surface);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}

