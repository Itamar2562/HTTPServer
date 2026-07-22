#include <stdio.h>
#include "contentUtils.h"
#include <string.h>
#include <stdint.h>


void initializeContent(Content *c)
{
    c->data=NULL;
    c->fileName=NULL;
    c->data_size=0;
    c->exists=0;
    c->type=NULL;
}

FILE *openFile(const char *filePath)
{
     FILE *f=fopen(filePath,"r");

    if (f==NULL){
    fprintf(stderr, "file not found\n");
        return NULL;
    }
     return f;
}

char *getFileExtension(char *filepath) {
    char *dot = strrchr(filepath, '.');
    if (!dot || dot == filepath) {
        return NULL; 
    }
    size_t len=strlen(dot+1);
    char *extension=(char *)malloc(len+ 1);
    strcpy(extension,dot+1);
    return extension;
}

char *getFileName(char *filePath)
{
    char *forwardSlash= strrchr(filePath, '/');
    if (!forwardSlash )
        return NULL;

    char *fileName;
    if (forwardSlash ==filePath)
    {
        fileName=(char *)malloc(strlen(forwardSlash)+1);
        strcpy(fileName,forwardSlash+1);
    }
    else{
    size_t len=strlen(forwardSlash+1);
    fileName=(char *)malloc(len+ 1);
    strcpy(fileName,forwardSlash+1);
    }
   
    return fileName;
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

Content *loadContent(char *filePath)
{   
    Content *c=(Content *)malloc(sizeof(Content) );
    initializeContent(c);        
    FILE *f=openFile(filePath);
    if (f==NULL)
        return c;

    long length=getFileSize(f);
    if (length<0 || (unsigned long)length>=SIZE_MAX )
        return NULL;

    c->data= (char *)malloc(sizeof(char)*length); 
     if (c->data==NULL)
        {
            fprintf(stderr,"memory error");
            return NULL;
        }
    size_t bytes_read= fread(c->data,1, length, f );

    if (bytes_read<length)
    {
        fprintf(stderr , "reading file error\n");
    }
    c->type=getFileExtension(filePath);
    c->fileName=getFileName(filePath);
    
    c->data_size=bytes_read;
    c->exists=1;
    fclose(f);
    return c;
}

void freeContent(Content *c)
{   
    if (c->data !=NULL)
        free(c->data);
    if (c->type!=NULL)
        free(c->type);
    if (c->fileName!=NULL)
        free(c->fileName);
    free(c);
}

