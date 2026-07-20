#include "HttpGet.h"
#include "../../HttpResponse/HttpResponse.h"
#include "../../../ContentLayer/contentUtils.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>


const char *ExtractHeaderFilePath(char *buffer)
{
  char * startPtr=strchr(buffer,'/');
  if (startPtr ==NULL) // didnt find it
    return NULL;
  size_t length=strcspn(startPtr, " "); //get the length until the second space

  char *filePath= (char *)malloc(length + 1);
  strncpy(filePath, startPtr,length);
  filePath[length]='\0';

  return filePath;
}

const char *buildCompleteFilePath(const char *path)
{
  char *completePath= (char *)malloc(strlen(path)+ strlen(FILE_PATH_START) +1);
  strcpy(completePath, FILE_PATH_START);
  strcat(completePath, path);

  return completePath;
}



int buildHttpGetResponse(httpResponse *r ,Content *c)
{
  if (c->data ==NULL)
    return 0;
  r->body= (char *)malloc(c->data_size +1 );
  strcpy(r->body,c->data);
  r->statusCode=200;
  r->body_length=c->data_size;

  char buffer[256];
  snprintf(buffer, sizeof(buffer), "%zu", r->body_length);

  addHeader(r->headersList, "HTTP/1.1 ", getStatusHeader( r->statusCode));
  addHeader(r->headersList,"Content-Length: ",buffer );
  addHeader(r->headersList,"Content-Type: ",c->type);
  addHeader(r->headersList, "Connection: ", "keep-alive");

  freeContent(c);
  return 1;
}


const char* ExtractCompleteHeaderPath(char *headers)
{
  const char *filePath=ExtractHeaderFilePath(headers);
  const char *completePath;
  if (strcmp(filePath, "/")==0)
    completePath=buildCompleteFilePath("index.html");
  else
    completePath=buildCompleteFilePath(filePath);
  free((char *)filePath);
  return completePath;
}

int GETResponse(httpResponse *r,char *headers)
{

  //make a content Struct that we will get from loadContent
 
  const char *filePath=ExtractCompleteHeaderPath(headers);
  Content *c= loadContent(filePath);
  free((char *)filePath);

  if (!c->exists)
  {
    const char *NotFoundfilePath=buildCompleteFilePath("NotFound.html");
    c=loadContent(NotFoundfilePath);
    free((char *)NotFoundfilePath);
  }
  
  int status= buildHttpGetResponse(r,c);
  return status;
}

