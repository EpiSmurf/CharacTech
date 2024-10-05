#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>

void to_uppercase(char *str)
{
	// this function takes a world changes it to a capital letter
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

void print_one_dim(char arr[], int size)
{

   if(size == 0)
   {
      printf("[]\n");
   }

   else
   {
      printf("[");

      for(int i = 0; i < size - 1; i++)
      {
          printf("%c, ",arr[i]);
      }
   
      printf("%d]\n", arr[size-1]);  
   } 
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

     to_uppercase(argv[0]);
     
     int len = my_strlen(argv[2]); //length of the world to be search

     FILE *fp;

     fp = fopen(argv[1], "r");
     
     if(fp == NULL)
     {
	     printf("Error: not such file");
	     return 1;
     }
        
    
    char grid[9][10];
    
    
    int i = 0;
    int j = 0;
    char charac;
    while ((charac = fgetc(fp)) != EOF)
    {
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



     print_two_dim(9,10,grid);
     printf("%i,%i\n",i,j);

     printf("Not found\n");
     return 0;

     	
}
