#pragma once
#include <stdlib.h>

#define DEFAULT_SITE "index.html"
#define NOT_FOUND_NAME "NotFound"
#define BAD_REQUEST_NAME "BadRequest"

typedef struct 
{
    char *data;
    size_t data_size;

    char *fileName;
    char *type;
    int exists;
} Content;


Content *loadContent(const char *filePath);
Content *getContentDetailsWithoutBody(const char *fullPath);
void freeContent(Content *c);



