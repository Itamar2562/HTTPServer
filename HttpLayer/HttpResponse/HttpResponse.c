#include "HttpResponse.h"
#include "../HttpHeader/HttpHeader.h"

int initializeHttpResponse(httpResponse * response)
{
    response->statusCode=200;
    response->body_length=0;
    response->body=NULL;
    response->headersList=(headerList *)malloc(sizeof(headerList));
    if (response->headersList ==NULL)
        return 0;
    int innerStatus= initializeHeaderList(response->headersList);

    return innerStatus;
}


void freeHttpResponse(httpResponse *response)
{
    freeHeaderList(response->headersList);
    free(response);
}

