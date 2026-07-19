#include "HttpResponse.h"
#include "../HttpHeader/HttpHeader.h"

int initializeHttpResponse(httpResponse * response)
{
    response->statusCode=200;
    response->body_length=0;
    response->body=NULL;
    initializeHeaderList(response->headers);

    return  response->headers!=NULL ?  1 : 0;
}


void freeHttpResponse(httpResponse *response)
{
    freeHeaderList(response->headers);
    free(response);
}

