#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ContentUtils.h"
//I removed filename for realPath bc realpath fails if file doesn't exists
//so instead I check if the dir itself exists and is safe
int isPathSafe(const char *userFilepath)
{   
    char *dir = omitFileName(userFilepath);
    if (dir== NULL)
        return 0;
    char *basePath = realpath(FILE_PATH_START, NULL);
    if (basePath ==NULL)
    {
        perror("realpath base");
        return 0;
    }
    char *fullPath = (char *)malloc(strlen(basePath) +1 + strlen(dir) +1 );
    if (fullPath ==NULL)
    {
        perror("fullPath");
        free(basePath);
        free(dir);
        return 0;
    }
    sprintf(fullPath, "%s/%s", basePath , dir);

    char *realFullPath = realpath(fullPath , NULL);
    free(fullPath);
    free(dir);
    if (realFullPath ==NULL)
    {
        perror ("realpath FullPath");
        return 0;
    }

    size_t baseLen= strlen(basePath);

    if (strncmp(realFullPath , basePath , baseLen) !=0 || 
    (realFullPath[baseLen] !='/' && realFullPath[baseLen] !='\0'))
    {
        perror("base paths aren't the same");
        free(basePath);
        free(realFullPath);
        return 0;
    }    
   free(realFullPath);
   free(basePath);
   return 1;
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
char *omitFileName(const char *filePath)
{
    size_t length;
    const char *lastSlash = strrchr(filePath, '/');
    if (lastSlash ==NULL)
        length=0;
    else
        length = lastSlash - filePath;
    char *fullDir=(char *)malloc(length +1);
    if (fullDir ==NULL)
        return NULL;
    memcpy(fullDir, filePath , length);
    fullDir[length]='\0';
    return fullDir;
}

FILE *openFile(const char *filePath ,const char *mode )
{
     FILE *f=fopen(filePath,mode);

    if (f==NULL){
    fprintf(stderr, "file not found\n");
        return NULL;
    }
     return f;
}

int writeToFile(const char *filePath , const char *data)
{
    FILE *f = openFile(filePath , "w+");

    int status = fprintf(f, "%s" , data);
    fclose(f);
    return status;
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

int hasFileExtension(const char *filePath){
    const char *dot=strrchr(filePath, '.');
        if (!dot || dot==filePath)
            return 0;
    return 1;
}

char *changeFileExtension(const char *filePath, const char *extension)
{
    size_t length=strcspn(filePath, ".");
    size_t extensionLength=strlen(extension);
    char *newPath=(char *)malloc(length + extensionLength +1 +1); //+1 for . and then for \0
    if (newPath ==NULL)
        return NULL;
    memcpy(newPath, filePath, length);
    memcpy (newPath +length , ".", 1);
    memcpy(newPath + length+1, extension, extensionLength);
    newPath[length +extensionLength+1]='\0';
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

