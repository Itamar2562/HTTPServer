#include "HttpRequest.h"
#include "../HttpHeader/HttpHeader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int initializeRequest(HttpRequest *request)
{
    request->headers=NULL;
    request->method=NULL;
    request->path=NULL;
    request->version=NULL;
}

void freeRequest(HttpRequest *request)
{
    free(request->headers);
    free(request->method);
    free(request->path);
    free(request->version);
    free(request);
}

char * extractHttpRequestMethod(char *headers, size_t *offset)
{
    size_t length=strcspn(headers, " "); //get the length until the first space
    char *buffer= (char *)malloc(length + 1);
    if (buffer ==NULL)
    return NULL;
    memcpy(buffer,headers, length);
    buffer[length]='\0';

    if (headers[length]=='\0') //didn't find whitespace
        (*offset) =length;
    else
        (*offset) =length+1 ; //skip the whitespace
    return buffer;
}

char *extractHeaderFilePath(char *header , size_t *offset)
{
    if (header==NULL || header[0]=='\0')
        return NULL;
    char * startPtr=strchr(header, '/'); //get the ptr to the first /
    if (startPtr ==NULL) // didnt find it
        return NULL;
    size_t length=strcspn(startPtr, " "); //get the length until the second space

    char *filePath= (char *)malloc(length + 1);
    if (filePath ==NULL)
        return NULL;
    strncpy(filePath, startPtr,length);
    filePath[length]='\0';
    if (startPtr[length]=='\0') //didn't find whitespace
        (*offset) =length;
    else
        (*offset) =length+1 ; //skip the whitespace
  return filePath;
}

char *extractHttpRequestVersion(char *headers, size_t *offset)
{
  size_t length=strcspn(headers, "\r\n"); //get the length until the end
  if (headers[length] =='\0' || headers[length] == '\n' ||(headers[length] == '\r' && headers[length+1] != '\n' ))
    return NULL;
  char *buffer= (char *)malloc(length + 1);
  if (buffer ==NULL)
    return NULL;
  memcpy(buffer,headers, length);
  buffer[length]='\0';
  (*offset) =length +2;
  return buffer;
}

HttpRequest *buildHttpRequest(char *request)
{
    size_t offset=0;
    char *start=request;
    HttpRequest *ParsedRequest= (HttpRequest *)malloc(sizeof(HttpRequest));
    initializeRequest(ParsedRequest);

    char *method=extractHttpRequestMethod(start , &offset);
    if (method ==NULL)
        {
            freeRequest(ParsedRequest);
            return NULL;
        }
    ParsedRequest->method=method;
    start+=offset;
    char *filePath=extractHeaderFilePath(start,&offset);
    if (filePath==NULL)
    {
        freeRequest(ParsedRequest);
        return NULL;
    }
    ParsedRequest->path=filePath;
    start += offset;
    char *version=extractHttpRequestVersion(start,&offset );
    if (version ==NULL)
    {
         freeRequest(ParsedRequest);
        return NULL;
    }
     ParsedRequest->version=version;
    start += offset;
  
    headerList *hl=buildHeaderListFromHTTPRequest(start);
    if (hl==NULL)
    {
            freeRequest(ParsedRequest);
            return NULL;
    }
    ParsedRequest->headers=hl;
}
