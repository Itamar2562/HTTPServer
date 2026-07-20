#include <stdio.h>
#include "contentUtils.h"
#include <string.h>
#include <stdint.h>


void initializeContent(Content *c)
{
    c->data=NULL;
    c->data_size=0;
    c->exists=0;
    c->type=NULL;
}

FILE *openFile(const char *filePath)
{
     FILE *f=fopen(filePath,"r");

    if (f==NULL){
    fprintf(stderr, "file not found");
        return NULL;
    }
     return f;
}

const char *getFileExtension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return ""; 
    }
    return dot + 1; 
}


long getFileSize(FILE *f)
{
     if (fseek(f,0,SEEK_END) ==-1) //find the offset 
     {
        perror("fseek end");
        return -1;
     }
    long length=ftell(f);
    if (length<0){
        perror("ftell");
        return -1;
    }

    if (fseek (f,0,SEEK_SET)==-1)
    {
        perror("fseek start");
        return -1;
    }
    return  length;
}

Content *loadContent(const char *filePath)
{   
    Content *c=(Content *)malloc(sizeof(Content) );
    initializeContent(c);        
    FILE *f=openFile(filePath);
    if (f==NULL)
        return c;

    long length=getFileSize(f);
    if (length<0 || (size_t)length==SIZE_MAX )
        return NULL;

    c->data= (char *)malloc(sizeof(char)*length +1); //+1 for null \0
     if (c->data==NULL)
        {
            fprintf(stderr,"memory error");
            return NULL;
        }
    size_t bytes_read= fread(c->data,1, length, f );
    c->data[bytes_read]='\0';

    c->type=getFileExtension(filePath);
    if (c->type=="")
        return NULL;

    c->data_size=bytes_read;
    c->exists=1;
    fclose(f);
    printf("%s\n",c->data);
    return c;
}

void freeContent(Content *c)
{   
    if (c->data !=NULL)
        free(c->data);
    free(c);
}

