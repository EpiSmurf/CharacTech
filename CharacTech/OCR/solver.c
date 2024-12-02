#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int is_white(Uint32 pixel_color, SDL_PixelFormat *format)
{
    Uint8 r,g,b;
    SDL_GetRGB(pixel_color, format,&r, &g , &b);

    if(r==255 && g==255 && b==255)
    {
       return 1;
    }

    return 0;


}

Uint32 pixel_to_color(Uint32 pixel_color, SDL_PixelFormat *format)
{
    Uint8 r,g,b;
    SDL_GetRGB(pixel_color, format,&r, &g , &b);

    return  SDL_MapRGB(format, 255, 182, 193);
}

void surface_to_color(SDL_Surface *surface)
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

    for (int y = 5; y < height - 5; ++y)
    {
        for (int x = 5; x < width - 5; ++x) 
        {
            Uint32 *pixel = &pixels[y * width + x];
            if (is_white(*pixel, format))
            {
                *pixel = pixel_to_color(*pixel, format);
            }
        }
    }

    SDL_UnlockSurface(surface);
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


void search(int col, int row,char grid[row][col], char* word)
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


int main(int argc, char *argv[])
{
    /*This is solver function:
     *argv[1] takes the file that contains the word grid
     *argv[2] takes the word that should be found on the grid
     and returns the position of the word in the grid we created*/
     

     if(argc != 4)
     {
	     err(EXIT_FAILURE,"you don't take two argument");
     }

     
     FILE *fp;

     fp = fopen(argv[1], "r");

     if(fp == NULL)
     {
	     printf("Error: not such file");
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

    search( col, row, grid, argv[2]); //search the word in the grid

// SDL_TEST

    if(SDL_Init(SDL_INIT_VIDEO)!=0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow("Surface_to_no_green", 0, 0, 0, 0,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* t = IMG_Load(argv[3]);
    if (t == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Surface *surface = SDL_ConvertSurfaceFormat(t, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(t);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_SetWindowSize(window,  surface->w, surface->h);

    surface_to_color(surface);

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

      

    return 0;
}
