#include "HttpGet.h"
#include "../../HttpResponse/HttpResponse.h"
#include "../../../ContentLayer/contentUtils.h"
#include "../../FilePathsHandler/filePaths.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>


const char *getHttpContentType(const char *fileExtention)
{
  if (strcmp(fileExtention,"")==0)
    return "application/octet-stream"; //default binary data

  else if (strcmp(fileExtention,"html")==0)
    return "text/html";

  else if (strcmp(fileExtention,"css")==0)
    return "text/css";
  
  else if (strcmp(fileExtention,"js")==0)
    return "text/javascript";

  else if (strcmp(fileExtention,"png")==0)
    return "image/png";
  
  else if (strcmp(fileExtention,"jpeg")==0)
    return "image/jpeg";

  else if (strcmp(fileExtention,"svg")==0)
    return "image/svg+xml";
  else
    return "text/plain";
}


int buildHttpGetResponse(httpResponse *r ,Content *c)
{
  r->body= (char *)malloc(c->data_size +1 );
  if (c->data ==NULL || r->body ==NULL)
    return 0;
  memcpy(r->body, c->data, c->data_size);
  r->statusCode=200;
  r->body_length=c->data_size;

  char buffer[256];
  snprintf(buffer, sizeof(buffer), "%zu", r->body_length);

  addHeader(r->headersList, "HTTP/1.1 ", getStatusHeader( r->statusCode));
  addHeader(r->headersList,"Content-Length: ",buffer );

  addHeader(r->headersList,"Content-Type: ",getHttpContentType(c->type));
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
  else if (strcmp(filePath, "/favicon.ico")==0)
      fullPath=getCompleteFilePath(SITE_ICON);
  else
    fullPath=getCompleteFilePath(filePath+1); //ignore the /

  

  Content *c= loadContent(fullPath);

  free(filePath);
  free(fullPath);

  if (c==NULL)
    return 0;

  if (!c->exists)
  {
    char *NotFoundfilePath=getCompleteFilePath("/NotFound.html");
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

