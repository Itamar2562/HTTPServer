#pragma once
#include "../HttpHeader/HttpHeader/HttpHeader.h"

typedef struct {

    char *method;
    char *path;
    char *fileType;
    char *version;

    headerList *headerList;
    
} HttpRequest;

HttpRequest *buildHttpRequest(char *request);



