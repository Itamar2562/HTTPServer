#include "HttpGet.h"
#include "../../../ContentLayer/contentUtils.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>


const char *getHttpContentType(const char *fileExtention)
{
  if (strcmp(fileExtention,"")==0) return "application/octet-stream"; //default binary data
  else if (strcmp(fileExtention,"html")==0) return "text/html";
  else if (strcmp(fileExtention,"css")==0)  return "text/css";
  else if (strcmp(fileExtention,"js")==0)   return "text/javascript";
  else if (strcmp(fileExtention,"png")==0)  return "image/png";
  else if (strcmp(fileExtention,"jpeg")==0) return "image/jpeg";
  else if (strcmp(fileExtention,"svg")==0)  return "image/svg+xml";
  else  return "text/plain";
}


int buildHttpGetResponse(httpResponse *r ,Content *c , int statusCode, char *version)
{
  r->body= (char *)malloc(c->data_size +1 );
  if (c->data ==NULL || r->body ==NULL)
    return 0;
  memcpy(r->body, c->data, c->data_size);
  r->statusCode=statusCode;
  r->body_length=c->data_size;
  r->version=version;

  char buffer[256];
  snprintf(buffer, sizeof(buffer), "%zu", r->body_length);

  addHeader(r->headersList,"Content-Length",buffer );
  snprintf(buffer, sizeof(buffer), "inline; filename=\"%s\"", c->fileName);
  addHeader(r->headersList,"Content-Disposition",buffer);

  addHeader(r->headersList,"Content-Type",getHttpContentType(c->type));
  addHeader(r->headersList, "Connection", "keep-alive");

  return 1;
}


Content * getContentByRequest(HttpRequest *request)
{
  char *fullPath;

  if (strcmp(request->path, "/")==0)
     fullPath=getCompleteFilePath(DEFAULT_SITE);
  else if (strcmp(request->path, "/favicon.ico")==0)
      fullPath=getCompleteFilePath(SITE_ICON);
  else
    fullPath=getCompleteFilePath(request->path+1); //ignore the /

  
  Content *c= loadContent(fullPath);

  free(fullPath);
  return c;
}

int validVersion(HttpRequest *request)
{
  if (strcmp(request->version,"HTTP/1.1")==0)
    return 0;
}


int GETResponse(httpResponse *response,HttpRequest *request)
{
 
  Content *c=getContentByRequest(request);

  if (c==NULL)
    return 0;

  int statusCode=200;

  if (!c->exists && validVersion(request))
  {
    char *NotFoundfilePath=getCompleteFilePath("NotFound.html");
    if (NotFoundfilePath ==NULL)
      return 0;
    freeContent(c);
    c=loadContent(NotFoundfilePath);
    free(NotFoundfilePath);
    statusCode=404;
  }
  int status= buildHttpGetResponse(response,c, statusCode,request->version);
  freeContent(c);
  return status;
}

