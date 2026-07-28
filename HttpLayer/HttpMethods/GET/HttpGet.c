#include "HttpGet.h"
#include "../../../ContentLayer/contentUtils.h"
#include "../../MimeTypes/MimeTypes.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>



int buildHttpGetResponse(httpResponse *r ,Content *c , int statusCode, char *version, const char *connetion)
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

  addHeader(r->headersList,"Content-Type",getHttpMimeType(c->type));
  addHeader(r->headersList, "Connection", connetion);

  return 1;
}

char *redirectToCorrectFullPath(char *path)
{
  char *fullPath;
  if (strcmp(path, "/")==0)
     fullPath=getCompleteFilePath(DEFAULT_SITE);
  else if (strcmp(path, "/favicon.ico")==0)
      fullPath=getCompleteFilePath(SITE_ICON);
  else
    fullPath=getCompleteFilePath(path+1); //ignore the /
  return fullPath;
}


int getNotValidResponse(httpResponse *response, HttpRequest *request)
{
    char *fullPath=getCompleteFilePath("VersionNotSupported.html");
    Content *c=loadContent(fullPath);

    if (c==NULL)
      return 0;
    int status= buildHttpGetResponse(response, c, 505, request->version, "close" );
    return status;
}

int GETResponse(httpResponse *response,HttpRequest *request)
{
  //check version if not correct return html
  char *fullPath= redirectToCorrectFullPath(request->path);
  //get accept settings
  //try the path if not found
  //get extention with the mimetypes change the fullpath extention to the first and loadContent again
  //in a loop if not found go to the next change fullpath extention
  //finally if not found return a 404.
  Content *c=loadContent(fullPath);

  if (c==NULL)
    return 0;

  int statusCode=200;

  if (!c->exists)
  {
    char *NotFoundfilePath=getCompleteFilePath("NotFound.html");
    if (NotFoundfilePath ==NULL)
      return 0;
    freeContent(c);
    c=loadContent(NotFoundfilePath);
    free(NotFoundfilePath);
    statusCode=404;
  }
  int status= buildHttpGetResponse(response,c, statusCode,request->version,"keep-alive");
  freeContent(c);
  return status;
}

