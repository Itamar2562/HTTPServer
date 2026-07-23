#pragma once
#include "../HttpHeader/HttpHeader.h"

typedef struct {

    char *method;
    char *path;
    char *version;

    headerList *headers;
    
} HttpRequest;

HttpRequest *buildHttpRequest(char *request);



