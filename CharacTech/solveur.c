#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>


int is_white(Uint32 pixel_color, SDL_PixelFormat *format)
{
    Uint8 r,g,b;
    SDL_GetRGB(pixel_color, format,&r, &g , &b);

    if(r >= 200 && g >= 200 && b >= 200)
    {
       return 1;
    }

    return 0;
}

Uint32 pixel_to_color(Uint32 pixel_color, SDL_PixelFormat *format,int color[])
{
    Uint8 r,g,b;
    SDL_GetRGB(pixel_color, format,&r, &g , &b);

    return  SDL_MapRGB(format, color[0], color[1], color[2]);
}

void surface_to_color(SDL_Surface *surface,int color[])
{
    if (SDL_LockSurface(surface) != 0) 
    {
        fprintf(stderr, "Unable to lock surface: %s\n", SDL_GetError());
        return;
    }

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    for (int y = 0 ; y < height ; ++y)
    {
        for (int x = 0 ; x < width ; ++x) 
        {
            Uint32 *pixel = &pixels[y * width + x];
            if (is_white(*pixel, format))
            {
                *pixel = pixel_to_color(*pixel, format,color);
            }
        }
    }

    SDL_UnlockSurface(surface);
}

SDL_Surface* loadImage(int i, int j) 
{
    char filename[256];
    snprintf(filename, sizeof(filename), "grid_char_%i_%i.png", i, j);

    SDL_Surface* img = IMG_Load(filename);
    if (!img) 
    {
        printf("Failed to load image: %s. SDL_image Error: %s\n", filename, IMG_GetError());
    }
    return img;
}


SDL_Surface* loadImage2(int i, int j, int k) 
{
    char filename[256];
    snprintf(filename, sizeof(filename), "word_%i_%i_%i.png", i, j,k);

    SDL_Surface* img = IMG_Load(filename);
    if (!img) 
    {
        printf("Failed to load image: %s. SDL_image Error: %s\n", filename, IMG_GetError());
    }
    return img;
}

void saveImage(SDL_Surface* img, int i, int j)
{
    char filename[256];
    snprintf(filename, sizeof(filename), "grid_char_%i_%i.png", i, j);

    if (IMG_SavePNG(img, filename) != 0)
    {
        printf("Failed to save image: %s. SDL_image Error: %s\n", filename, IMG_GetError());
    }
    else
    {
        printf("Image saved successfully as: %s\n", filename);
    }
}

void saveImage2(SDL_Surface* img, int i, int j, int k) 
{
    char filename[256];
    snprintf(filename, sizeof(filename), "word_%i_%i_%i.png", i, j,k);

    if (IMG_SavePNG(img, filename) != 0) 
    {
        printf("Failed to save image: %s. SDL_image Error: %s\n", filename, IMG_GetError());
    }
    else 
    {
        printf("Image saved successfully as: %s\n", filename);
    }
}

void to_uppercase(char *str)
{
	// this function takes a world change it to a capital letter
        int i = 0;

        while( str[i] != '\0')
        {

                if(str[i] >= 'a' && str[i] <= 'z')
                {
                        str[i] = str[i] - 32;
                }

                i++;
        }

}

int my_strlen(char str[])
{
       //this function return the length of word
       int i = 0;

       while(str[i] != '\0')
       {
             i = i + 1;
       }

       return i;
}


void search(int col, int row,char grid[row][col], char* word,int color[])
{
       // this fonction search on the grid thei position of word of find
       int check = 0; //check if the word was found

       to_uppercase(word);
       int len = my_strlen(word) -1; //length of the world to be search
       for(int i = 0; i< row; i++)
       {
	   for(int j = 0; j<col; j++)
	   {
	       if(word[0] == grid[i][j])
	       {
		  if(i+len <= col && word[len] == grid[i+len][j])
		  {
		     //vertical case
                     int r = i +1;
                     int w = 1;
		     while(w<len-1 && word[w] == grid[r][j])
		     {
		           r++;
			   w++;
		     }

		     if(word[w] == grid[r][j])
                     {
			 for(int l = i; l<=i+len; l++)
			 {
			      SDL_Surface* t =  loadImage(l,j);
			      surface_to_color(t,color);
			      saveImage(t, l,j);
			      SDL_FreeSurface(t);
			 }
			 printf("(%i,%i),(%i,%i)\n",j,i,j,i+len);
			 check = 1;
			 break;
	             }
	          }
			
		 if(j+len <= row && word[len] == grid[i][j+len])
		 {
		    //horizontal case
                    int c = j +1; 
                    int w = 1;   
                    while(w<len-1 && word[w] == grid[i][c])
		    {  
		           c++;
                           w++;
                    }

                    if(word[w] == grid[i][c])
                    {
		       for(int l = j; l<=j+len; l++)
                       {
                              SDL_Surface* t =  loadImage(i,l);
                              surface_to_color(t, color);
                              saveImage(t, i,l);
                              SDL_FreeSurface(t);
                       }
                       printf("(%i,%i),(%i,%i)\n",j,i,j+len,i);
		       check = 1;
                       break;
                    }
		}


		if(i-len >= 0 && word[len] == grid[i-len][j])
                {
                   //inverse vertical case
                   int r = i - 1;
                   int w = 1;
                   while(w<len-1 && word[w] == grid[r][j])
                   {
                          r--;
                          w++;
                   }
       
		   if(word[w] == grid[r][j])
                   {
		      for(int l = i; l>=i-len; l--)
                      {
                              SDL_Surface* t =  loadImage(l,j);
                              surface_to_color(t, color);
                              saveImage(t, l,j);
                              SDL_FreeSurface(t);
                      }
                      printf("(%i,%i),(%i,%i)\n",j,i,j,i-len);
                      check = 1;
                      break;
                   }
                }

		if(j-len >= 0 && word[len] == grid[i][j-len])
                {
                   //inverse horizontal case
                   int c = j -1;
                   int w = 1;
                   while(w<len-1 && word[w] == grid[i][c])
                   {
                          c--;
                          w++;
                   }

                   if(word[w] == grid[i][c])
                   {
		      for(int l = j; l>=j-len; l--)
                      {
                              SDL_Surface* t =  loadImage(i,l);
                              surface_to_color(t, color);
                              saveImage(t, i,l);
                              SDL_FreeSurface(t);
                      }
                      printf("(%i,%i),(%i,%i)\n",j,i,j-len,i);
		      check = 1;
                      break;
                   }
                }


		if(i+len <= row && j+len <= col 
	        && word[len] == grid[i+len][j+len])
                {
                   //diagonal case
                   int c = j + 1;
              	   int r = i + 1;
                   int w = 1;
                   while(w<len-1 && word[w] == grid[r][c])
                   {
                         c++;
		         r++;
                         w++;
                   }

                   if(word[w] == grid[r][c])
                   {
		      for (int d = 0; d <= len; d++) // d représente le déplacement le long de la diagonale
		      {
    			   int c = i + d; // Coordonnée verticale
    			   int l = j + d;
			   SDL_Surface* t =  loadImage(c,l);
                           surface_to_color(t, color);
                           saveImage(t, c, l);
                           SDL_FreeSurface(t);
		      }
                      printf("(%i,%i),(%i,%i)\n",j,i,j+len,i+len);
		      check = 1;
                      break;
                   }
                 }

		 if(i+len <= row && j-len <= col 
	         && word[len] == grid[i+len][j-len])
                 {
                    //diagonale 2 case 
       	            int c = j - 1;
                    int r = i + 1;
                    int w = 1;
                    while(w<len-1 && word[w] == grid[r][c])
                    {
                          c--;
                          r++;
                          w++;
                    }

                    if(word[w] == grid[r][c])
                    {
		       for (int d = 0; d <= len; d++) // d représente le déplacement le long de la diagonale
                       {
                           int c = i + d; // Coordonnée verticale
                           int l = j - d;
                           SDL_Surface* t =  loadImage(c,l);
                           surface_to_color(t, color);
                           saveImage(t, c, l);
                           SDL_FreeSurface(t);
                       } 
                       printf("(%i,%i),(%i,%i)\n",j,i,j-len,i+len);
		       check = 1;
                       break;
                    }
                  }

	        if(i-len <= row && j-len <= col 
		&& word[len] == grid[i-len][j-len])
                {
                  //inverse diagonal case
                  int c = j - 1;
                  int r = i - 1;
                  int w = 1;
                  while(w<len-1 && word[w] == grid[r][c])
		  {
                         c--;
                         r--;
                         w++;
                  }

                  if(word[w] == grid[r][c])
                  {
		     for (int d = 0; d <= len; d++) // d représente le déplacement le long de la diagonale
                     {
                           int c = i - d; // Coordonnée verticale
                           int l = j - d;
                           SDL_Surface* t =  loadImage(c,l);
                           surface_to_color(t, color);
                           saveImage(t, c, l);
                           SDL_FreeSurface(t);
                     }
                     printf("(%i,%i),(%i,%i)\n",j,i,j-len,i-len);
		     check = 1;
                     break;
                  }
                }

		if(i-len <= row && j+len <= col 
	        && word[len] == grid[i-len][j+len])
                {
                   //inverse diagonal 2 case
                   int c = j + 1;
                   int r = i - 1;
                   int w = 1;
                   while(w<len-1 && word[w] == grid[r][c])
                   {
                         c++;
                         r--;
                         w++;
                   }

                   if(word[w] == grid[r][c])
		   {
		      for (int d = 0; d <= len; d++) // d représente le déplacement le long de la diagonale
                      {
                           int c = i - d; // Coordonnée verticale
                           int l = j + d;
                           SDL_Surface* t =  loadImage(c,l);
                           surface_to_color(t, color);
                           saveImage(t, c, l);
                           SDL_FreeSurface(t);
                      }
                      printf("(%i,%i),(%i,%i)\n",j,i,j+len,i-len);
		      check = 1;
                      break;
                   }
                }

	      }
	  }

          if(check==1)
          {
	     break;
          }
       }

       if(check!=1)
       {
         printf("Not found\n");
       }

}



int main()
{
    /*This is solver function:
     *argv[1] takes the file that contains the word grid
     *argv[2] takes the word that should be found on the grid
     and returns the position of the word in the grid we created*/
     

//.......... part of read the grid file........//
   
     FILE *fp;

     fp = fopen("grid", "r");

     if(fp == NULL)
     {
	     printf("Error: no such file\n");
	     return 1;
     }
        
    int row = 0;
    int col = 0;
    char c;

    while ((c = fgetc(fp)) != EOF)
    {
        // this loop tranforms the file on a grid of char

        if (c == '\n')
        {
                row++;
        }

        else if (c >= 'A' && c <='Z' && row==0)
        {
                col++;
        }
    }
     
    rewind(fp);  // Move the cursor back to the beginning of the file

    char grid[row][col];
    int i = 0;
    int j = 0;
    while ((c = fgetc(fp)) != EOF)
    {
	// this loop tranforms the file on a grid of char

	if (c == '\n')
        {
                j = 0;
                i++;
        }

	else if (c >= 'A' && c <='Z')
        {
                grid[i][j] = c;
                j++;
        }
    }

    fclose(fp);
//.......... part of read the grid file........//
     FILE *fp2;

     fp2 = fopen("word", "r");

     if(fp2 == NULL)
     {
             printf("Error: no such file\n");
             return 1;
     }

    int line = 0;
    while ((c = fgetc(fp2)) != EOF)
    {
        // this loop count the nomber of word to be search

        if (c == '\n')
	{
                line++;
        }
    }
    
    rewind(fp2);  // Move the cursor back to the beginning of the file 
    
    char* word[line];
    char buffer[50];
    i = 0;
    while (fscanf(fp2, "%49s", buffer) != EOF)
    {
        // this loop tranforms the file on a grid of char
        word[i] = malloc(strlen(buffer) );
	strcpy(word[i], buffer);
	i++;
    }
    fclose(fp2);
//.......... end of read both file ...........//
 
for(int i = 0; i< row; i++)
{
    for(int j = 0; j<col; j++)
    {
	SDL_Surface* t =  loadImage(i,j);
     	saveImage(t, i,j);
     	SDL_FreeSurface(t);
    }
}
 // printf("row: %i,col: %i\n",row,col);
 
int color[9][3]= {
        	{254, 191, 210},
        	{49, 140, 231},
		{150, 131, 236},
		{135, 233, 144},
		{221, 152, 92},
		{250, 234, 115},
		{187, 210, 225},
		{204, 255, 255},
		{255, 99, 71},
            };

 for(int k=0; k<line; k++)
 {
    search(col, row, grid, word[k],color[k%9]); //search the word in the grid
					 //
 }
// SDL_PART: il reconstitue l'image avec la resolution des mot trouver
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Fusion", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) 
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
    int temp_size = 0;
    SDL_Surface* result = NULL;
    for (int i = 0; i < row; i++) // Loop through rows
    {
 	   for (int j = 0; j < col; j++) // Loop through columns
    	   {
        	SDL_Surface* img2 = loadImage(i, j); // Load image for each cell
        	if (!img2) continue; // Skip if the image cannot be loaded

        	if (result == NULL)
        	{
            		// Create result surface for the first image
            		result = SDL_CreateRGBSurface(0, img2->w * col, img2->h * row, 32, 0, 0, 0, 0);
        	}
	
        	// Blit the image onto the correct spot in the grid
        	SDL_Rect destRect;
        	destRect.x = j * img2->w; // X position based on column
        	destRect.y = i * img2->h; // Y position based on row

        	// Copy the image onto the result surface
        	SDL_BlitSurface(img2, NULL, result, &destRect);
            temp_size = img2->w; 
        	SDL_FreeSurface(img2); // Free img2 after use
    	   }
    }
    int x = 0;
    int y = 0;
    int z = 0;
    int current_nb_word = 0;
    char file_name[50];

    snprintf(file_name, sizeof(file_name), "word_%i_%i_%i.png", x, y, z);
    while( access(file_name, F_OK) == 0)
    {
        while( access(file_name, F_OK) == 0)
        {
            for(z = 0; z < my_strlen(word[current_nb_word]); z++)
            {
                SDL_Surface* img2 = loadImage2(x, y,z); // Load image for each cell
        	    if (!img2) continue; // Skip if the image cannot be loaded

        	    if (result == NULL)
        	    {
            		// Create result surface for the first image
            		result = SDL_CreateRGBSurface(0, img2->w * col, img2->h * row, 32, 0, 0, 0, 0);
        	    }
	
            	// Blit the image onto the correct spot in the grid
            	SDL_Rect destRect;
            	destRect.x = col*temp_size; // X position based on column
            	destRect.y =  img2->h * current_nb_word; // Y position based on row

            	// Copy the image onto the result surface
            	SDL_BlitSurface(img2, NULL, result, &destRect);

            	SDL_FreeSurface(img2); // Free img2 after use
            }
            current_nb_word+=1;
            y+=1;
        }
        y=0;
        x+=1;
        snprintf(file_name, sizeof(file_name), "word_%i_%i_%i.png", x, y, z);
    }

    if (result)
    {
        SDL_BlitSurface(result, NULL, screenSurface, NULL);
        SDL_UpdateWindowSurface(window);

        SDL_Event event;
        while (1)
        {
            SDL_WaitEvent(&event);
            if (event.type == SDL_QUIT)
                break;
        }

        SDL_FreeSurface(result); // Libérer la surface résultante
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
