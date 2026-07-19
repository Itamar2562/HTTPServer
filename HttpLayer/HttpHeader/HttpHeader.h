#pragma once
#include <stdlib.h>

#include "HttpHeader.h"

#define MAX_HEADER_COUNT 5

typedef struct
{
    char *key;
    char *value;
} header;

typedef struct 
{
    header *headers;
    int count;
    int max_size;
    size_t total_byte_length;
} headerList;

int initializeHeaderList(headerList *headers);
void freeHeaderList(headerList *hl);
