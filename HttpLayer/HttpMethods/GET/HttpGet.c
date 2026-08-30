#include "HttpGet.h"
#include "../../../ContentLayer/ContentLoader/ContentLoader.h"
#include "../../../ContentLayer/ContentUtils/ContentUtils.h"
#include "../../MimeTypes/MimeTypes.h"
#include "../../HttpHeader/HttpHeadersParameters/HttpHeaderParam/paramList/paramList.h"
#include "../../requestPreferences/requestPreferences.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>


int buildHttpGetResponse(httpResponse *r ,Content *c , int statusCode, char *version, const char *connetion)
{
  r->body= (char *)malloc(c->data_size +1 );
  if (r->body ==NULL)
    return 0;
  if (c->data !=NULL)
    memcpy(r->body, c->data, c->data_size);
  else
    r->body = NULL;
  r->statusCode=statusCode;
  r->body_length=c->data_size;
  if (version ==NULL)
    version = DEFAULT_HTTP_VERSION;
  r->version= (char *)malloc(strlen(version) +1);
  if (r->version ==NULL)
    {
      free(r->body);
      return 0;
    }
  strcpy(r->version,version);

  addDefaultHTTPOnePointOneHeaders(r,getHttpMimeType(c->type) , connetion );

  return 1;
}

int getNotValidResponse(httpResponse *response, HttpRequest *request)
{
    const char* filePath= "VersionNotSupported.html";
    char *fullPath=getCompleteFilePath(filePath, DEFAULT_PATH);
    if (fullPath ==NULL )
    {
      free(fullPath);
      return 0;
    }
    Content *c=loadContent(fullPath);
    if (c==NULL){
      free(fullPath);
      return 0;
    }
    int status= buildHttpGetResponse(response, c, 505, DEFAULT_HTTP_VERSION, "close" );
    free(fullPath);
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
    char* filePath= changeFileExtension(NOT_FOUND_NAME, "html"); //fallback to html
    char *fullPath=getCompleteFilePath(filePath, DEFAULT_PATH);
    if (fullPath ==NULL || filePath == NULL){
      free (filePath);
      free(fullPath);
      return 0;
    }
    c= loadContent(fullPath);
    if (c==NULL)
    {
      free (filePath);
      free(fullPath);
      return 0;
    }
  }
  int status = buildHttpGetResponse(response, c, 404,request->version, "keep-alive");
  return status;
}

httpResponse  *getBadRequestResponse()
{
    httpResponse * response = (httpResponse *)malloc(sizeof(httpResponse));
    if (response == NULL || initializeHttpResponse(response) ==0)
     return 0;
    char* filePath= changeFileExtension(BAD_REQUEST_NAME, "html"); //fallback to html
    char *fullPath=getCompleteFilePath(filePath, DEFAULT_PATH);
    if (fullPath ==NULL || filePath == NULL)
    {
      free (filePath);
      free(fullPath);
      return 0;
    }
    Content *c= loadContent(fullPath);
    if (c==NULL){
      free (filePath);
      free(fullPath);
      return 0;
    }
  int status = buildHttpGetResponse(response, c, 400,NULL, "close");
  free (filePath);
  free(fullPath);
  if (!status)
    return NULL;
  return response;
}

int GETResponse(httpResponse *response,HttpRequest *request)
{
  //check version if not correct return html
  const char *filePath= redirectToCorrectPath(request->path);
  //printHeaders(request->headerList);

    if (!isPathSafe(filePath , DEFAULT_PATH))
        return 0;
    char *fullPath=getCompleteFilePath(filePath, DEFAULT_PATH);
    if (fullPath ==NULL)
        return 0;

  Content *c=loadContent(fullPath);
  if (c==NULL)
  {
    free(fullPath);
    return getNotFoundResponse (response, request);
  }

  if (!c->exists) 
  {
    c=getContentBasedOnAcceptHeader(fullPath , request);
    if (c==NULL)
    {
      free(fullPath);
      int status = getNotFoundResponse (response, request);
      return status;
    }
  }

  int status= buildHttpGetResponse(response,c, 200,request->version,"keep-alive");
  freeContent(c);
  free(fullPath);
  return status;
}




