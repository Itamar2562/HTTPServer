#pragma once
#include <stdlib.h>


#define FILE_PATH_START "ContentLayer/content/"
#define DEFAULT_SITE "index.html"
#define SITE_ICON "home.png"


typedef struct 
{
    char *data;
    size_t data_size;

    char *fileName;
    char *type;
    int exists;
} Content;


Content *loadContent(char *filePath);
void freeContent(Content *c);
char *getCompleteFilePath(const char *path);





