#include <stdio.h>
#include "contentUtils.h"
#include <string.h>



char* loadContent(char *name)
{   
    int startingLength=10;
    FILE *f=fopen(name,"r");

    if (f==NULL){
    printf("file not found");
        return NULL;
    }
     

    if (fseek(f,0,SEEK_END) ==-1) //find the offset 
        perror("fseek end");
    long length=ftell(f);
    if (length==-1){
        perror("ftell");
        return NULL;
    }

    if (fseek (f,0,SEEK_SET)==-1)
        perror("fseek start");

    char *buffer= (char *)malloc(sizeof(char)*length +1); //+1 for null \0
     if (buffer==NULL)
        {
            printf("body buffer memory error");
            return NULL;
        }
    size_t read_bytes= fread(buffer, sizeof(char), length, f);
    buffer[read_bytes]='\0'; // make it a c string
    fclose(f);
    return buffer;
}
