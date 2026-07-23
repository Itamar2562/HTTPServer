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
} headerList;

int initializeHeaderList(headerList *headers);
void freeHeaderList(headerList *hl);
void addHeader(headerList *headerList, const char *key, const char *value);
int RemoveHeader(headerList *headerList, const char *key);
const char *getStatusHeader(int statusCode);
char *findHeaderValue(headerList *hl, char *key);
char *buildHTTPHeadersFromHeaderList(headerList *hl, size_t *headersLength);
void printHeaders(headerList *headerList);
headerList* buildHeaderListFromHTTPRequest(char *headers);

