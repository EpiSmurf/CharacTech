#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>

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
       int i = 0;

       while(str[i] != '\0')
       {
             i = i + 1;
       }

       return i;
}

void print_two_dim(int row, int col, char arr[row][col])
{
   if(row == 0 && col == 0)
   {
      printf("[]\n");
   }

   else
   {
      printf("[\n");

      for(int i = 0; i < row; i++)
      {  
         printf(" [");

         for(int j = 0; j < col - 1; j++)
         {
            printf("%i, ",arr[i][j]);
         }
         
            printf("%i]\n", arr[i][col-1]);
      }

      printf("]\n" );
   } 
}



void search(int col, int row,char grid[row][col], char* word)
{
       // this fonction search on the grid the word of find

       //int x0 = 0;
       //int x1 = 0;
       //int y0 = 0; 
       //int y1 = 0; 

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
				 //cas vertical
                                 int r = i +1;
			
                                 int w = 1;
				 while( w<len-1 && word[w] == grid[r][j])
			         {
					 r++;
					 w++;
				 }

				 if(word[w] == grid[r][j])
			         {
					 printf("(%i,%i),(%i,%i)\n",i,j,j,i+len);
					 break;
			         }
		              }
			
			      if(j+len <= row && word[len] == grid[i][j+len])
			      {
				 //cas horizontal
                                   int c = j +1; 
                                   int w = 1;   

				 while( w<len-1 && word[w] == grid[i][c])
                                 {
                                         c++;
                                         w++;
                                 }

                                 if(word[w] == grid[i][c])
                                 {
                                         printf("(%i,%i),(%i,%i)\n",i,j,j+len,i);
                                         break;
                                 }
			      }

			      if(j+len <= row && word[len] == grid[i][j+len])
                              {
                                 //cas diagonale a fair 
                                   int c = j +1;
                                   int w = 1;

                                 while( w<len-1 && word[w] == grid[i][c])
                                 {
                                         c++;
                                         w++;
                                 }

                                 if(word[w] == grid[i][c])
                                 {
                                         printf("(%i,%i),(%i,%i)\n",i,j,j+len,i);
                                         break;
                                 }
                              }
		      }
	      }
       }


       printf("Not found\n");
}


int main(int argc, char *argv[])
{
    /*This is the function solver:
     * he has two parameter
     * the first takes the file that contain the grid of word
     * the second takes the word has found on the grid*/

     argc = 2;
     if(argv[argc] == NULL)
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
    char charac;

    while ((charac = fgetc(fp)) != EOF)
    {
        // this loop tranforms the file on a grid of char

        if (charac == '\n')
        {
                row++;
        }

        else if (charac >= 'A' && charac<='Z' && row==0)
        {
                col++;
        }
    }
     
     rewind(fp);  // Remettre le curseur au début du fichier

     printf("%i,%i\n",row,col); 
     char grid[row][col];
    
    
    int i = 0;
    int j = 0;

    while ((charac = fgetc(fp)) != EOF)
    {
	// this loop tranforms the file on a grid of char

	if (charac == '\n')
        {
                j = 0;
                i++;
        }

	else if (charac >= 'A' && charac<='Z')
        {
                grid[i][j] = charac;
                j++;
        }
    }

     fclose(fp);



     print_two_dim(row,col,grid);
     
     search( col, row, grid, argv[2]);

     return 0;

     	
}
