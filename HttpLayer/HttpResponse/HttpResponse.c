#include "HttpResponse.h"
#include "../HttpHeader/HttpHeader.h"
#include <stdio.h>
#include <string.h>
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
    free(response->version);
    free(response);
}

char *buildHttpResponseLine(httpResponse *r, size_t *length)
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s %s\r\n", r->version,getStatusHeader(r->statusCode));

    (*length)=strlen(buffer);
    char *responseLine= (char *)malloc((*length) +1);
    if (responseLine ==NULL)
        return NULL;
    strcpy(responseLine, buffer);
}

char *buildCompleteResponse(httpResponse *r, size_t *fullResponseLength)
{
    if (r==NULL || fullResponseLength==NULL)
        return NULL;
    size_t responseLineLength=0;
    char *responseLine= buildHttpResponseLine(r, &responseLineLength);
    size_t responseHeadersLength=0;
    char *responseHeaders=buildHTTPHeadersFromHeaderList(r->headersList,&responseHeadersLength);
    if (responseHeaders ==NULL)
        return NULL;
    printf("response headers: %s\n",responseHeaders);
    (*fullResponseLength)=responseLineLength+responseHeadersLength + r->body_length;
    char *fullResponse =(char *)malloc(*fullResponseLength);

    if (responseHeaders==NULL || fullResponse ==NULL)
    return NULL;

    size_t offset=0;
    memcpy(fullResponse, responseLine, responseLineLength);
    offset+= responseLineLength;
    memcpy(fullResponse +offset, responseHeaders, responseHeadersLength);
    offset +=  responseHeadersLength;
    memcpy(fullResponse +offset, r->body , r->body_length);

    free(responseHeaders);
    return fullResponse;
}