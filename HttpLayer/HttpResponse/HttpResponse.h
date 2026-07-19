#pragma once

#include <stdlib.h>
#include "HttpResponse.h"
#include "../HttpHeader/HttpHeader.h"


typedef struct 
{
    char *status_code;
    
    headerList *headers;

    size_t body_length;
    char *body;
}  httpResponse;


char *buildHTTPHeaders(size_t contentLength, int statusCode);
int initializeHttpResponse(httpResponse * response);
void freeHttpResponse(httpResponse *response);




