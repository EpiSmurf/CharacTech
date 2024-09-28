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

size_t my_strlen(const char str[])
{
       size_t conteur=0;
       int i = 0;

       while(str[i] != '\0')
       {
             conteur = conteur + 1;
             i = i + 1;
       }

       return conteur;
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
     
     //char grid[2][2];

     FILE *fp;

     fp = fopen(argv[1], "r");
     
     if(fp == NULL)
     {
	     printf("Error: not such file");
	     return 1;
     }
     
     char str[1000];
     int i, j = 0;

     while(fgets(str, 1000, fp)!= NULL)
     {
	     printf("%s",str);
	     i++;
             j++;	     
     }

    printf("%i,%i\n",i,j);
    fclose(fp); 
    

    printf("Not found\n");
    return 0;

     	
}
