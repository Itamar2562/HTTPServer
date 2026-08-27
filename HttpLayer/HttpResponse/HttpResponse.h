#pragma once

#include <stdlib.h>
#include "HttpResponse.h"
#include "../HttpHeader/HttpHeader/HttpHeader.h"

#define DEFAULT_HTTP_VERSION "HTTP/1.1"

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
void addDefaultHTTPOnePointOneHeaders(httpResponse *r , const char *mimeType , const char *connectionType);


void addContentLengthHeader(httpResponse *r);




