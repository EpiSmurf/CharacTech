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

void function(const char* imagePath);
void execute_command(char *argv[]);
void make_in_directory(const char *dir);

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error initializing SDL:\n");
        return 1;
    }

    // Initializing SDL_ttf
    if (TTF_Init() == -1) {
        printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("User Interface", 
		    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		    SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        printf("Error creating window: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
		    SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Error initializing SDL_image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Loading font "DejaVu"
    TTF_Font* font = TTF_OpenFont("/nix/store/gmqq69j1nzx7j58f469y2cg0xm09d2vg-dejavu-fonts-minimal-2.37/share/fonts/truetype/DejaVuSans.ttf", 24);
    if (!font) {
        printf("Error loading font: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Variables for the interface
    char inputText[256] = "";
    int inputActive = 0;
    SDL_Texture* imageTexture = NULL;
    SDL_Rect imageRect = {0, 0, 0, 0}; 
    char imagePath[256] = ""; 

    // Define buttons
    SDL_Rect buttonSelectImage = {50, 50, 200, 50};
    SDL_Rect buttonExecute = {300, 50, 200, 50};
    SDL_Rect buttonRotate = {550, 50, 200, 50};

    SDL_Rect inputRect = {50, 120, 700, 50};
    int IsRotated = 0;
    // Total height of the elements (buttons and input field)
    int uiHeight = inputRect.y + inputRect.h; 

    // Colors
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Color buttonColor = {216, 220, 254, 255};

    // Main loop
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_TEXTINPUT:
                    if (inputActive && strlen(inputText) < 255) {
                        strcat(inputText, event.text.text);
                    }
                    break;
                case SDL_KEYDOWN:
                    if (inputActive) {
                        if (event.key.keysym.sym == SDLK_BACKSPACE && 
					strlen(inputText) > 0) {
                            inputText[strlen(inputText) - 1] = '\0';
                        } else if (event.key.keysym.sym == SDLK_RETURN) {
                            if (imageTexture) {
                                SDL_DestroyTexture(imageTexture);
                                imageTexture = NULL;
                            }
                            SDL_Surface* imageSurface = IMG_Load(inputText);
                            if (!imageSurface) {
                                printf("Error loading image: %s\n",
						IMG_GetError());
                            } else {
                                strncpy(imagePath, inputText, sizeof(imagePath));
                                imagePath[sizeof(imagePath) - 1] = '\0';

                                imageRect.w = imageSurface->w;
                                imageRect.h = imageSurface->h;

                                // Resize if the image is too large for the window
                                if (imageRect.w > SCREEN_WIDTH || 
						imageRect.h > (SCREEN_HEIGHT - uiHeight - 20)) {
                                    float widthRatio = (float)SCREEN_WIDTH /
					    imageRect.w;
                                    float heightRatio = (float)(SCREEN_HEIGHT -
						    uiHeight - 20) / 
					    imageRect.h;
                                    float scale = widthRatio < heightRatio 
					    ? widthRatio : heightRatio;

                                    imageRect.w = (int)(imageRect.w * scale);
                                    imageRect.h = (int)(imageRect.h * scale);
                                }

                                // Position the image below the elements
                                imageRect.x = (SCREEN_WIDTH - imageRect.w) / 2;
                                imageRect.y = uiHeight + 10; 

                                imageTexture = SDL_CreateTextureFromSurface(
						renderer, imageSurface);
                                SDL_FreeSurface(imageSurface);
                            }
                            // Deactivate the input field
                            inputActive = 0;
                            SDL_StopTextInput();
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int x = event.button.x;
                        int y = event.button.y;

                        // Click on "Select Image" button
                        if (x >= buttonSelectImage.x && x <= 
					buttonSelectImage.x + 
					buttonSelectImage.w &&
                            y >= buttonSelectImage.y && y <= 
			    buttonSelectImage.y + buttonSelectImage.h) {
                            // Activate the input field to enter the image path
                            inputActive = 1;
                            SDL_StartTextInput();
                            // Clear text
                            inputText[0] = '\0';
                        }
                        // Click on "Execute" button
                        else if (x >= buttonExecute.x && x <= buttonExecute.x 
					+ buttonExecute.w &&
                                 y >= buttonExecute.y && y <= buttonExecute.y 
				 + buttonExecute.h) {
                            if (strlen(imagePath) > 0) {
                                // Execute the function
                                if(IsRotated) function("rotated.png");
                                else { function(imagePath); }
                            } else {
                                printf("Select an image before executing.\n");
                            }
                        }
                        // Click on "Rotate" button
                        else if (x >= buttonRotate.x && x <= buttonRotate.x + 
					buttonRotate.w &&
                                 y >= buttonRotate.y && y <= buttonRotate.y + 
				 buttonRotate.h) {
                            // Check if an image has been loaded
                            if (strlen(imagePath) > 0) {
                                // Execute the rotation function with the image path
                                make_in_directory("./Pretreatment/Rotation");
                                char *cmd1[] =
				{"./Pretreatment/Rotation/rotation", (char *)
					imagePath, NULL};
                                execute_command(cmd1);

                                // Load the rotated image
                                if (imageTexture) {
                                    SDL_DestroyTexture(imageTexture);
                                    imageTexture = NULL;
                                }
                                SDL_Surface* imageSurface = IMG_Load("rotated.png");
                                if (!imageSurface) {
                                    printf("Error loading rotated image: %s\n", 
						    IMG_GetError());
                                } else {
                                    imageRect.w = imageSurface->w;
                                    imageRect.h = imageSurface->h;

                                    // Resize if the image is too large
                                    if (imageRect.w > SCREEN_WIDTH || 
						    imageRect.h > (
							    SCREEN_HEIGHT - 
							    uiHeight - 20)) {
                                        float widthRatio = (float)SCREEN_WIDTH
						/ imageRect.w;
                                        float heightRatio = (float)(
							SCREEN_HEIGHT - 
							uiHeight - 20) / 
						imageRect.h;
                                        float scale = widthRatio < heightRatio 
						? widthRatio : heightRatio;

                                        imageRect.w = (int)(imageRect.w * 
							scale);
                                        imageRect.h = (int)(imageRect.h * 
							scale);
                                    }

                                    // Position the image
                                    imageRect.x = (SCREEN_WIDTH - imageRect.w)
					    / 2;
                                    imageRect.y = uiHeight + 10; 

                                    imageTexture = 
					    SDL_CreateTextureFromSurface(
							    renderer, 
							    imageSurface);
                                    SDL_FreeSurface(imageSurface);

                                    // Update image path to the rotated image
                                    strncpy(imagePath, "rotated.png", 
						    sizeof(imagePath));
                                    imagePath[sizeof(imagePath) - 1] = '\0';
                                    IsRotated = 1;
                                }
                            } else {
                                printf("Select an image before rotating.\n");
                            }
                        }
                        else {
                            break;
                        }
                    }
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Display the image 
        if (imageTexture) {
            SDL_RenderCopy(renderer, imageTexture, NULL, &imageRect);
        }

        // Draw "Select Image" button
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, 
			buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &buttonSelectImage);

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, 
			"Select Image", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(
			    renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {buttonSelectImage.x + 10, 
			buttonSelectImage.y + 10, textSurface->w, 
			textSurface->h};
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            } else {
                printf("Error creating text texture: %s\n", SDL_GetError());
            }
            SDL_FreeSurface(textSurface);
        } else {
            printf("Error rendering text: %s\n", TTF_GetError());
        }

        // Draw "Execute" button
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, 
			buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &buttonExecute);

        textSurface = TTF_RenderText_Solid(font, "Execute", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, 
			    textSurface);
            if (textTexture) {
                SDL_Rect textRect = {buttonExecute.x + 50, buttonExecute.y + 
			10, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            } else {
                printf("Error creating text texture\n");
            }
            SDL_FreeSurface(textSurface);
        } else {
            printf("Error rendering text: %s\n", TTF_GetError());
        }

        // Draw "Rotate" button
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, 
			buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &buttonRotate);

        textSurface = TTF_RenderText_Solid(font, "Rotate", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, 
			    textSurface);
            if (textTexture) {
                SDL_Rect textRect = {buttonRotate.x + 50, buttonRotate.y + 10,
			textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            } else {
                printf("Error creating text texture: %s\n", SDL_GetError());
            }
            SDL_FreeSurface(textSurface);
        } else {
            printf("Error rendering text: %s\n", TTF_GetError());
        }

        // Input field
        if (inputActive) {
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderFillRect(renderer, &inputRect);

            // Entered text
            if (strlen(inputText) > 0) {
                textSurface = TTF_RenderText_Solid(font, inputText, textColor);
                if (textSurface) {
                    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(
				    renderer, textSurface);
                    if (textTexture) {
                        SDL_Rect textRect = {inputRect.x + 10, inputRect.y + 
				10, textSurface->w, textSurface->h};
                        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                        SDL_DestroyTexture(textTexture);
                    } else {
                        printf("Error creating text texture: %s\n", 
					SDL_GetError());
                    }
                    SDL_FreeSurface(textSurface);
                } else {
                    printf("Error rendering text: %s\n", TTF_GetError());
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

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

// execute a command
void execute_command(char *argv[]) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork error");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child: execute the command
        execvp(argv[0], argv);
        perror("Error executing command");
        exit(EXIT_FAILURE);
    } else {
        // Parent: wait for the command to finish
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Command %s failed\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

// Function to make in a directory
void make_in_directory(const char *dir) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork error");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Change directory and run 'make'
        if (chdir(dir) != 0) {
            perror("Error changing directory");
            exit(EXIT_FAILURE);
        }
        char *make_cmd[] = {"make", NULL};
        execvp("make", make_cmd);
        perror("Error executing make");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Make in directory %s failed\n", dir);
            exit(EXIT_FAILURE);
        }
    }
}

// Function to solve the OCR
void function(const char* imagePath) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork error");
        return;
    } else if (pid == 0) {

        // List of directories containing Makefiles
        const char *dirs[] = {
            ".",
            "./Pretreatment",
            "./Pretreatment/Rotation",
            "./Detection",
            "./NeuralNetwork",
	    "./Solver",
            NULL             
        };

        // Run make in each directory**
        for (int i = 0; dirs[i] != NULL; i++) {
            printf("Running make in directory %s\n", dirs[i]);
            make_in_directory(dirs[i]);
        }

        // Executing each steps
	
        char *cmd1[] = {"./Pretreatment/code", (char *)imagePath, NULL};
        execute_command(cmd1);

        char *cmd2[] = {"./Detection/detection", NULL};
        execute_command(cmd2);

        char *cmd3[] = {"./NeuralNetwork/neural_network", NULL};
        execute_command(cmd3);

        char *cmd4[] = {"./Solver/display", "grid", NULL};
        execute_command(cmd4);

        exit(EXIT_SUCCESS);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Child process finished with status %d\n", 
			    WEXITSTATUS(status));
        } else {
            printf("Child process did not terminate properly\n");
        }
    }
}
