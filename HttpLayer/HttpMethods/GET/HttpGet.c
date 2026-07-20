#include "HttpGet.h"
#include "../../HttpResponse/HttpResponse.h"
#include "../../../ContentLayer/contentUtils.h"
#include "../../FilePathsHandler/filePaths.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>






int buildHttpGetResponse(httpResponse *r ,Content *c)
{
  r->body= (char *)malloc(c->data_size +1 );
  if (c->data ==NULL || r->body ==NULL)
    return 0;
 
  strcpy(r->body,c->data);
  r->statusCode=200;
  r->body_length=c->data_size;

  char buffer[256];
  snprintf(buffer, sizeof(buffer), "%zu", r->body_length);

  addHeader(r->headersList, "HTTP/1.1 ", getStatusHeader( r->statusCode));
  addHeader(r->headersList,"Content-Length: ",buffer );
  addHeader(r->headersList,"Content-Type: ",c->type);
  addHeader(r->headersList, "Connection: ", "keep-alive");

  return 1;
}


int GETResponse(httpResponse *r,char *headers)
{

  //make a content Struct that we will get from loadContent
 
 char *filePath=extractHeaderFilePath(headers);
  if (filePath==NULL)
    return 0;

  char *fullPath;

  if (strcmp(filePath, "/")==0)
     fullPath=getCompleteFilePath(DEFAULT_SITE);
  else
    fullPath=getCompleteFilePath(filePath);

  

  Content *c= loadContent(fullPath);

  free(filePath);
  free(fullPath);

  if (c==NULL)
    return 0;

  if (!c->exists)
  {
    char *NotFoundfilePath=getCompleteFilePath("NotFound.html");
    if (NotFoundfilePath ==NULL)
      return 0;
    free(c);
    c=loadContent(NotFoundfilePath);
    free(NotFoundfilePath);
  }
  
  int status= buildHttpGetResponse(r,c);
  freeContent(c);
  return status;
}

