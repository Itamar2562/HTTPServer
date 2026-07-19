#pragma once

#include <stdlib.h>
#include "HttpResponse.h"
#include "../HttpHeader/HttpHeader.h"


typedef struct 
{
    int statusCode;
    headerList *headers;

    size_t body_length;
    char *body;

}  httpResponse;


int initializeHttpResponse(httpResponse * response);
void freeHttpResponse(httpResponse *response);




