#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void execute_command(char *argv[]);
void generate_new_path(const char *imagePath, const char *suffix, char *newPath, size_t newPathSize);
void turn_image(char *imagePath);
void filter_image(char *imagePath);
void cut_image(char *imagePath);
void process_image(char *imagePath);

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("Erreur lors de l'initialisation de SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Interface Utilisateur", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        printf("Erreur lors de la création de la fenêtre: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur lors de la création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur lors de l'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("/nix/store/gmqq69j1nzx7j58f469y2cg0xm09d2vg-dejavu-fonts-minimal-2.37/share/fonts/truetype/DejaVuSans.ttf", 24);
    if (!font) {
        printf("Erreur lors du chargement de la police: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    char inputText[256] = "";
    SDL_Texture* imageTexture = NULL;
    SDL_Rect imageRect = {0, 0, 0, 0};
    char imagePath[256] = "";

    SDL_Rect buttonDisplayImage = {50, 50, 200, 50};
    SDL_Rect buttonExecute = {300, 50, 200, 50};
    SDL_Rect buttonQuit = {550, 50, 200, 50};
    SDL_Rect buttonTurn = {50, 120, 200, 50};
    SDL_Rect buttonFilter = {300, 120, 200, 50};
    SDL_Rect buttonCut = {550, 120, 200, 50};
    SDL_Rect inputRect = {50, 190, 700, 50};
    int uiHeight = inputRect.y + inputRect.h;

    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Color buttonColor = {128, 72, 134, 255};

    int running = 1;
    SDL_Event event;

    SDL_StartTextInput();
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } 
            else if (event.type == SDL_TEXTINPUT) {
                if (strlen(inputText) < sizeof(inputText) - 1) {
                    strcat(inputText, event.text.text);
                }
            } 
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0) {
                    inputText[strlen(inputText) - 1] = '\0';
                }
            } 
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int x = event.button.x;
                    int y = event.button.y;

                    if (x >= buttonDisplayImage.x && x <= buttonDisplayImage.x + buttonDisplayImage.w &&
                        y >= buttonDisplayImage.y && y <= buttonDisplayImage.y + buttonDisplayImage.h) {
                        if (imageTexture) {
                            SDL_DestroyTexture(imageTexture);
                            imageTexture = NULL;
                        }

                        SDL_Surface* imageSurface = IMG_Load(inputText);
                        if (!imageSurface) {
                            printf("Erreur lors du chargement de l'image: %s\n", IMG_GetError());
                        } else {
                            strncpy(imagePath, inputText, sizeof(imagePath));
                            imagePath[sizeof(imagePath) - 1] = '\0';

                            imageRect.w = imageSurface->w;
                            imageRect.h = imageSurface->h;

                            if (imageRect.w > SCREEN_WIDTH || imageRect.h > (SCREEN_HEIGHT - uiHeight - 20)) {
                                float widthRatio = (float)SCREEN_WIDTH / imageRect.w;
                                float heightRatio = (float)(SCREEN_HEIGHT - uiHeight - 20) / imageRect.h;
                                float scale = (widthRatio < heightRatio) ? widthRatio : heightRatio;

                                imageRect.w = (int)(imageRect.w * scale);
                                imageRect.h = (int)(imageRect.h * scale);
                            }

                            imageRect.x = (SCREEN_WIDTH - imageRect.w) / 2;
                            imageRect.y = uiHeight + 10;

                            imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
                            SDL_FreeSurface(imageSurface);
                        }
                    } 
                    else if (x >= buttonExecute.x && x <= buttonExecute.x + buttonExecute.w &&
                             y >= buttonExecute.y && y <= buttonExecute.y + buttonExecute.h) {
                        if (strlen(imagePath) > 0) {
                            process_image(imagePath);

                            if (imageTexture) {
                                SDL_DestroyTexture(imageTexture);
                                imageTexture = NULL;
                            }

                            SDL_Surface* updatedImageSurface = IMG_Load(imagePath);
                            if (!updatedImageSurface) {
                                printf("Erreur lors du chargement de l'image mise à jour: %s\n", IMG_GetError());
                            } else {
                                imageRect.w = updatedImageSurface->w;
                                imageRect.h = updatedImageSurface->h;

                                if (imageRect.w > SCREEN_WIDTH || imageRect.h > (SCREEN_HEIGHT - uiHeight - 20)) {
                                    float widthRatio = (float)SCREEN_WIDTH / imageRect.w;
                                    float heightRatio = (float)(SCREEN_HEIGHT - uiHeight - 20) / imageRect.h;
                                    float scale = (widthRatio < heightRatio) ? widthRatio : heightRatio;

                                    imageRect.w = (int)(imageRect.w * scale);
                                    imageRect.h = (int)(imageRect.h * scale);
                                }

                                imageRect.x = (SCREEN_WIDTH - imageRect.w) / 2;
                                imageRect.y = uiHeight + 10;

                                imageTexture = SDL_CreateTextureFromSurface(renderer, updatedImageSurface);
                                SDL_FreeSurface(updatedImageSurface);
                            }
                        } else {
                            printf("Veuillez entrer un chemin d'image avant d'exécuter.\n");
                        }
                    } 
                    else if (x >= buttonTurn.x && x <= buttonTurn.x + buttonTurn.w &&
                             y >= buttonTurn.y && y <= buttonTurn.y + buttonTurn.h) {
                        if (strlen(imagePath) > 0) {
                            turn_image(imagePath);

                            if (imageTexture) {
                                SDL_DestroyTexture(imageTexture);
                                imageTexture = NULL;
                            }

                            SDL_Surface* updatedImageSurface = IMG_Load(imagePath);
                            if (!updatedImageSurface) {
                                printf("Erreur lors du chargement de l'image mise à jour: %s\n", IMG_GetError());
                            } else {
                                imageRect.w = updatedImageSurface->w;
                                imageRect.h = updatedImageSurface->h;

                                if (imageRect.w > SCREEN_WIDTH || imageRect.h > (SCREEN_HEIGHT - uiHeight - 20)) {
                                    float widthRatio = (float)SCREEN_WIDTH / imageRect.w;
                                    float heightRatio = (float)(SCREEN_HEIGHT - uiHeight - 20) / imageRect.h;
                                    float scale = (widthRatio < heightRatio) ? widthRatio : heightRatio;

                                    imageRect.w = (int)(imageRect.w * scale);
                                    imageRect.h = (int)(imageRect.h * scale);
                                }

                                imageRect.x = (SCREEN_WIDTH - imageRect.w) / 2;
                                imageRect.y = uiHeight + 10;

                                imageTexture = SDL_CreateTextureFromSurface(renderer, updatedImageSurface);
                                SDL_FreeSurface(updatedImageSurface);
                            }
                        } else {
                            printf("Veuillez entrer un chemin d'image avant de tourner.\n");
                        }
                    } 
                    else if (x >= buttonFilter.x && x <= buttonFilter.x + buttonFilter.w &&
                             y >= buttonFilter.y && y <= buttonFilter.y + buttonFilter.h) {
                        if (strlen(imagePath) > 0) {
                            filter_image(imagePath);

                            if (imageTexture) {
                                SDL_DestroyTexture(imageTexture);
                                imageTexture = NULL;
                            }

                            SDL_Surface* updatedImageSurface = IMG_Load(imagePath);
                            if (!updatedImageSurface) {
                                printf("Erreur lors du chargement de l'image mise à jour: %s\n", IMG_GetError());
                            } else {
                                imageRect.w = updatedImageSurface->w;
                                imageRect.h = updatedImageSurface->h;

                                if (imageRect.w > SCREEN_WIDTH || imageRect.h > (SCREEN_HEIGHT - uiHeight - 20)) {
                                    float widthRatio = (float)SCREEN_WIDTH / imageRect.w;
                                    float heightRatio = (float)(SCREEN_HEIGHT - uiHeight - 20) / imageRect.h;
                                    float scale = (widthRatio < heightRatio) ? widthRatio : heightRatio;

                                    imageRect.w = (int)(imageRect.w * scale);
                                    imageRect.h = (int)(imageRect.h * scale);
                                }

                                imageRect.x = (SCREEN_WIDTH - imageRect.w) / 2;
                                imageRect.y = uiHeight + 10;

                                imageTexture = SDL_CreateTextureFromSurface(renderer, updatedImageSurface);
                                SDL_FreeSurface(updatedImageSurface);
                            }
                        } else {
                            printf("Veuillez entrer un chemin d'image avant de filtrer.\n");
                        }
                    } 
                    else if (x >= buttonCut.x && x <= buttonCut.x + buttonCut.w &&
                             y >= buttonCut.y && y <= buttonCut.y + buttonCut.h) {
                        if (strlen(imagePath) > 0) {
                            cut_image(imagePath);
                        } else {
                            printf("Veuillez entrer un chemin d'image avant de découper.\n");
                        }
                    } 
                    else if (x >= buttonQuit.x && x <= buttonQuit.x + buttonQuit.w &&
                             y >= buttonQuit.y && y <= buttonQuit.y + buttonQuit.h) {
                        running = 0;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        if (imageTexture) {
            SDL_RenderCopy(renderer, imageTexture, NULL, &imageRect);
        }

        // Render Display Image Button
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &buttonDisplayImage);

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Display Image", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    buttonDisplayImage.x + (buttonDisplayImage.w - textSurface->w) / 2,
                    buttonDisplayImage.y + (buttonDisplayImage.h - textSurface->h) / 2,
                    textSurface->w,
                    textSurface->h
                };
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }

        // Render Execute Button
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &buttonExecute);

        textSurface = TTF_RenderText_Solid(font, "Execute", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    buttonExecute.x + (buttonExecute.w - textSurface->w) / 2,
                    buttonExecute.y + (buttonExecute.h - textSurface->h) / 2,
                    textSurface->w,
                    textSurface->h
                };
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }

        // Render Quit Button
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &buttonQuit);

        textSurface = TTF_RenderText_Solid(font, "Quit", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    buttonQuit.x + (buttonQuit.w - textSurface->w) / 2,
                    buttonQuit.y + (buttonQuit.h - textSurface->h) / 2,
                    textSurface->w,
                    textSurface->h
                };
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }

        // Render Turn Button
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &buttonTurn);

        textSurface = TTF_RenderText_Solid(font, "Turn", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    buttonTurn.x + (buttonTurn.w - textSurface->w) / 2,
                    buttonTurn.y + (buttonTurn.h - textSurface->h) / 2,
                    textSurface->w,
                    textSurface->h
                };
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }

        // Render Filter Button
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &buttonFilter);

        textSurface = TTF_RenderText_Solid(font, "Filter", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    buttonFilter.x + (buttonFilter.w - textSurface->w) / 2,
                    buttonFilter.y + (buttonFilter.h - textSurface->h) / 2,
                    textSurface->w,
                    textSurface->h
                };
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }

        // Render Cut Button
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &buttonCut);

        textSurface = TTF_RenderText_Solid(font, "Cut", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    buttonCut.x + (buttonCut.w - textSurface->w) / 2,
                    buttonCut.y + (buttonCut.h - textSurface->h) / 2,
                    textSurface->w,
                    textSurface->h
                };
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }

        // Render Input Box Background
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &inputRect);

        // Render Input Text
        if (strlen(inputText) > 0) {
            textSurface = TTF_RenderText_Solid(font, inputText, textColor);
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                if (textTexture) {
                    SDL_Rect textRect = {inputRect.x + 10, inputRect.y + 10, textSurface->w, textSurface->h};
                    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                    SDL_DestroyTexture(textTexture);
                }
                SDL_FreeSurface(textSurface);
            }
        }

        SDL_RenderPresent(renderer);
    }

    // Cleanup and exit
    if (imageTexture) {
        SDL_DestroyTexture(imageTexture);
    }
    TTF_CloseFont(font);
    IMG_Quit();
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void execute_command(char *argv[]) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("Erreur de fork");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {
        execvp(argv[0], argv);
        perror("Erreur lors de l'exécution de la commande");
        exit(EXIT_FAILURE);
    } 
    else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("Erreur de waitpid");
            exit(EXIT_FAILURE);
        }
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "La commande %s a échoué\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

void generate_new_path(const char *imagePath, const char *suffix, char *newPath, size_t newPathSize) {
    char *dot = strrchr(imagePath, '.');
    if (dot) {
        size_t base_length = dot - imagePath;
        if (base_length + strlen(suffix) + strlen(dot) + 1 > newPathSize) {
            newPath[0] = '\0';
            return;
        }
        strncpy(newPath, imagePath, base_length);
        newPath[base_length] = '\0';
        strcat(newPath, suffix);
        strcat(newPath, dot);
    }
    else {
        if (strlen(imagePath) + strlen(suffix) +1 > newPathSize) {
            newPath[0] = '\0';
            return;
        }
        strcpy(newPath, imagePath);
        strcat(newPath, suffix);
    }
}

void turn_image(char *imagePath) {
    char newPath[512];
    generate_new_path(imagePath, "_turned", newPath, sizeof(newPath));
    execute_command((char *[]){"./turn", imagePath, NULL});
    strncpy(imagePath, newPath, 256);
    imagePath[255] = '\0';
}

void filter_image(char *imagePath) {
    char newPath[512];
    generate_new_path(imagePath, "_filtered", newPath, sizeof(newPath));
    execute_command((char *[]){"./filter", imagePath, NULL});
    strncpy(imagePath, newPath, 256);
    imagePath[255] = '\0';
}

void cut_image(char *imagePath) {
    execute_command((char *[]){"./cut", imagePath, NULL});
}

void process_image(char *imagePath) {
    turn_image(imagePath);
    filter_image(imagePath);
    cut_image(imagePath);
}
