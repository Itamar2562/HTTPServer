#pragma once
#include <stdlib.h>

typedef struct 
{
    char *data;
    size_t data_size;

    char *type;
    int exists;
} Content;


Content *loadContent(char *filePath);
void freeContent(Content *c);





