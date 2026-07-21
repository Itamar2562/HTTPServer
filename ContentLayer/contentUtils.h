#pragma once
#include <stdlib.h>

typedef struct 
{
    char *data;
    size_t data_size;

    const char *type;
    int exists;
} Content;


Content *loadContent(const char *filePath);
void freeContent(Content *c);





