#include <stdio.h>
#include <stdlib.h>
#include <err.h>

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
     

     if(argc != 3)
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

    return 0;
}
