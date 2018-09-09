#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
#define FILENAMELENGTH 30
#define LINELENGTH 60000
 
int main()
{
    FILE *openfile, *openfile2;
    char file1[FILENAMELENGTH], file2[FILENAMELENGTH], line[LINELENGTH];
 
    printf("Enter file to copy: ");
    fgets(file1, FILENAMELENGTH, stdin);
    file1[strlen(file1) - 1] = '\0';
    printf("Enter file to copy to: ");
    fgets(file2, FILENAMELENGTH, stdin);
    file2[strlen(file2) - 1] = '\0';
 
    if((openfile = fopen(file1, "rb")) != NULL)
       printf("%s opened successfully\n", file1);
    else
    {
        perror("Error");
        exit(1);
    }
 
    if((openfile2 = fopen(file2, "wr")) != NULL)
        printf("%s opened successfully\n", file2);
    else
    {
        perror("Error");
        exit(1);
    }
 
    while(fread(line, 1, LINELENGTH, openfile) == LINELENGTH)
        if((fwrite(line, 1, LINELENGTH, openfile2)) != LINELENGTH)
        {
            puts("Error writing to file");
            exit(1);
        }
 
    fclose(openfile);
    fclose(openfile2);
 
    return 0;
}