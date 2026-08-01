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

char *getFileExtension(const char *filepath)
 {
    const char *dot = strrchr(filepath, '.');
    if (!dot || dot == filepath) {
        return NULL; 
    }
    size_t len=strlen(dot+1);
    char *extension=(char *)malloc(len+ 1);
    if (extension ==NULL)
        return NULL;
    strcpy(extension,dot+1);
    return extension;
}

char *changeFileExtension(char *filePath, const char *extension)
{
    size_t length=strcspn(filePath, ".");
    if (filePath[length]=='\0') //no dot
        return NULL; 
    size_t extensionLength=strlen(extension);
    char *newPath=(char *)malloc(length + extensionLength +1);
    if (newPath ==NULL)
        return NULL;
    memcpy(newPath, filePath, length);
    memcpy(newPath, extension, extensionLength);
    newPath[length +extensionLength]='\0';
    return newPath;
}

char *getFileName(const char *filePath)
{
    const char *forwardSlash= strrchr(filePath, '/');
    if (!forwardSlash )
        return NULL;

    char *fileName;
    if (forwardSlash ==filePath)
    {
        fileName=(char *)malloc(strlen(forwardSlash)+1);
        if (fileName ==NULL)
            return NULL;
        strcpy(fileName,forwardSlash+1);
    }
    else{
    size_t len=strlen(forwardSlash+1);
    fileName=(char *)malloc(len+ 1);
    if (fileName==NULL)
        return NULL;
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

char *getCompleteFilePath(const char *path)
{
  char *completePath= (char *)malloc(strlen(path)+ strlen(FILE_PATH_START) +1);
  if (completePath ==NULL)
    return NULL;
  strcpy(completePath, FILE_PATH_START);
  strcat(completePath, path);

  return completePath;
}

Content *loadContent(const char *filePath)
{   
    if (filePath ==NULL)
        return NULL;
    char *fullPath=getCompleteFilePath(filePath);
    if (fullPath ==NULL)
        return NULL;

    Content *c=(Content *)malloc(sizeof(Content) );
    if (c== NULL)
    {
        free(fullPath);
        return NULL;
    }
    initializeContent(c);        
    FILE *f=openFile(fullPath);
    if (f==NULL) //file not found
    {
        free(fullPath);
        return c;
    }
    long length=getFileSize(f);
    if (length<0 || (unsigned long)length>=SIZE_MAX )
        return NULL;

    c->data= (char *)malloc(sizeof(char)*length); 
     if (c->data==NULL)
        {
            fprintf(stderr,"memory error");
            freeContent(c);
            free(fullPath);
            return NULL;
        }
    size_t bytes_read= fread(c->data,1, length, f );

    if (bytes_read<length)
    {
        fprintf(stderr , "reading file error\n");
        freeContent(c);
        free(fullPath);
        return NULL;
    }
    c->type=getFileExtension(fullPath);
    c->fileName=getFileName(fullPath);

    if (c->fileName ==NULL || c->type ==NULL)
    {
        freeContent(c);
        return NULL;
    }
    
    c->data_size=bytes_read;
    c->exists=1;
    fclose(f);
    free(fullPath);
    return c;
}

void freeContent(Content *c)
{   
    free(c->data);
    free(c->type);
    free(c->fileName);
    free(c);
}

