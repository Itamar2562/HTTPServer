#include "HttpHead.h"
#include "../../../ContentLayer/ContentUtils/ContentUtils.h"
#include "../../MimeTypes/MimeTypes.h"
#include "../../HttpHeader/HttpHeadersParameters/HttpHeaderParam/paramList/paramList.h"
#include "../../requestPreferences/requestPreferences.h"
#include <string.h>
int buildHttpHEADResponse(httpResponse *r ,Content *c , int statusCode, char *version, const char *connetion)
{
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


Content *getHeadContentBasedOnAcceptHeader(const char * filePath, HttpRequest *request)
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

        Content *c=getContentDetailsWithoutBody(fullPath);
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

int getHeadNotFoundResponse(httpResponse *response,HttpRequest *request)
{
  Content *c=getHeadContentBasedOnAcceptHeader(NOT_FOUND_NAME , request);  //first search by accept
  if (c==NULL)
  {
    char* filePath= changeFileExtension(NOT_FOUND_NAME, "html"); //fallback to html
    char *fullPath=getCompleteFilePath(filePath, DEFAULT_PATH);
    if (fullPath ==NULL || filePath == NULL){
      free (filePath);
      free(fullPath);
      return 0;
    }
    c= getContentDetailsWithoutBody(fullPath);
    if (c==NULL)
    {
      free (filePath);
      free(fullPath);
      return 0;
    }
  }
  int status = buildHttpHEADResponse(response, c, 404,request->version, "keep-alive");
  return status;
}

int HEADResponse(httpResponse *response,HttpRequest *request)
{
  //check version if not correct return html
  const char *filePath= redirectToCorrectPath(request->path);
  printHeaders(request->headerList);

    if (!isPathSafe(filePath , DEFAULT_PATH))
        return 0;
    char *fullPath=getCompleteFilePath(filePath, DEFAULT_PATH);
    if (fullPath ==NULL)
        return 0;

  Content *c=getContentDetailsWithoutBody(fullPath);
  if (c==NULL)
  {
    free(fullPath);
    return getHeadNotFoundResponse (response, request);
  }

  if (!c->exists) 
  {
    c=getHeadContentBasedOnAcceptHeader(fullPath , request);
    if (c==NULL)
    {
      free(fullPath);
      int status = getHeadNotFoundResponse (response, request);
      return status;
    }
  }
   int status = buildHttpHEADResponse(response, c, 200,request->version, "keep-alive");
  return status;
}