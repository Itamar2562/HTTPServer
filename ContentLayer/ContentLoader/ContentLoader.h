#pragma once
#include <stdlib.h>

#define DEFAULT_SITE "index.html"
#define NOT_FOUND_NAME "NotFound"

typedef struct 
{
    char *data;
    size_t data_size;

    char *fileName;
    char *type;
    int exists;
} Content;


Content *loadContent(const char *filePath);
void freeContent(Content *c);
char *getFileExtension(const char *filepath);
char *getFileName(const char *filePath);
char *changeFileExtension(const char *filePath, const char *extension);





