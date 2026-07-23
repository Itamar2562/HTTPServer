#pragma once

#include <stdlib.h>
#include "HttpResponse.h"
#include "../HttpHeader/HttpHeader.h"


typedef struct 
{
    int statusCode;
    char *version;
    headerList *headersList;

    size_t body_length;
    char *body;

}  httpResponse;


int initializeHttpResponse(httpResponse * response);
void freeHttpResponse(httpResponse *response);
char *buildCompleteResponse(httpResponse *r, size_t *fullResponseLength);




