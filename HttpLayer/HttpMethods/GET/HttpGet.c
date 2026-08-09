#include "HttpGet.h"
#include "../../../ContentLayer/contentUtils.h"
#include "../../MimeTypes/MimeTypes.h"
#include "../../HttpHeader/HttpHeadersParameters/HttpHeaderParam/paramList/paramList.h"
#include "../../requestPreferences/requestPreferences.h"
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

  r->version= (char *)malloc(strlen(version) +1);
  if (r->version ==NULL)
    {
      free(r->body);
      return 0;
    }
  strcpy(r->version,version);

  char buffer[256];
  snprintf(buffer, sizeof(buffer), "%zu", r->body_length);

  addHeader(r->headersList,"Content-Length",buffer );
  snprintf(buffer, sizeof(buffer), "inline; filename=\"%s\"", c->fileName);
  addHeader(r->headersList,"Content-Disposition",buffer);

  addHeader(r->headersList,"Content-Type",getHttpMimeType(c->type));
  addHeader(r->headersList, "Connection", connetion);

  return 1;
}

const char *redirectToCorrectPath(char *path)
{
  if (strcmp(path, "/")==0)
     return DEFAULT_SITE;
  // else if (strcmp(path, "/favicon.ico")==0)
  //   return SITE_ICON;
  else
    return path+1; //ignore the /
}


int getNotValidResponse(httpResponse *response, HttpRequest *request)
{
    Content *c=loadContent("VersionNotSupported.html");
    if (c==NULL)
      return 0;
    int status= buildHttpGetResponse(response, c, 505, request->version, "close" );
    return status;
}

Content *getContentBasedOnAcceptHeader(const char * filePath, HttpRequest *request)
{
   
    paramList *pl =getAcceptMimeTypeParamList(request);
    if (pl==NULL)
      return NULL;

    for (int i=0; i< pl->param_count ; i++)
    {
      extensionList *el = getExtensionByMimeType (pl->parameters[i].parameter ); 
      if (el ==NULL)
        continue;
      for (int i=0; i<el->length; i++)
      {
        char *fullPath = changeFileExtension(filePath , el->extensions[i]);
        if (fullPath ==NULL)
          continue;
        printf("%s\n",fullPath);

        Content *c=loadContent(fullPath);
        free(fullPath);

        if (c==NULL)
        {
          freeParamList(pl);
          freeExtensionList(el);
          return NULL;
        }
        if (c->exists)
        {
          freeExtensionList(el);
          freeParamList(pl);
          return c;
        }
      }
      freeExtensionList(el);
    }
    freeParamList(pl);
    return NULL;
}

int getNotFoundResponse(httpResponse *response,HttpRequest *request)
{
  Content *c=getContentBasedOnAcceptHeader(NOT_FOUND_NAME , request);  //first search by accept
  if (c==NULL)
  {
    char* fullpath= changeFileExtension(NOT_FOUND_NAME, "html"); //fallback to html
    c= loadContent(fullpath);
    if (c==NULL)
      return 0;
  }
  int status = buildHttpGetResponse(response, c, 404,request->version, "keep-alive");
  return status;
}


int GETResponse(httpResponse *response,HttpRequest *request)
{
  //check version if not correct return html
  const char *filePath= redirectToCorrectPath(request->path);
  printHeaders(request->headerList);

  Content *c=loadContent(filePath);
  if (c==NULL)
    return getNotFoundResponse (response, request);

  if (!c->exists) 
  {
    c=getContentBasedOnAcceptHeader(filePath , request);
    if (c==NULL)
    {
      int status = getNotFoundResponse (response, request);
      return status;
    }
  }

  int status= buildHttpGetResponse(response,c, 200,request->version,"keep-alive");
  freeContent(c);
  return status;
}

