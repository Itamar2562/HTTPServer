#include "HttpGet.h"
#include "../../../ContentLayer/contentUtils.h"
#include "../../MimeTypes/MimeTypes.h"
#include "../../HttpHeader/HttpHeadersParameters/HttpHeaderParam/paramList/paramList.h"
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
  else if (strcmp(path, "/favicon.ico")==0)
      return SITE_ICON;
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

int GETResponse(httpResponse *response,HttpRequest *request)
{
  //check version if not correct return html
  const char *filePath= redirectToCorrectPath(request->path);
  printHeaders(request->headerList);
  char *RawAcceptParameters= getHeaderValue(request->headerList, "Accept");
  paramList *pl= parseParameterizedHeader(RawAcceptParameters);
  printParameterList(pl);
  freeParamList(pl);

  //get accept settings
  //try the path if not found
  //get extention with the mimetypes change the fullpath extention to the first and loadContent again
  //in a loop if not found go to the next change fullpath extention
  //finally if not found return a 404.

  //get accept settings and try the provided path if it contains a mim type
  //if not found/donsn't work find the biggest q and search and try it. if not working try the same q
  //if no q found lower the q and try again.
  //if found return else return doesn't exists based again on the accept defaults to html
  Content *c=loadContent(filePath);



  if (c==NULL)
    return 0;

  int statusCode=200;

  if (!c->exists)
  {
    freeContent(c);
    c=loadContent("NotFound.html");
    statusCode=404;
  }
  int status= buildHttpGetResponse(response,c, statusCode,request->version,"keep-alive");
  freeContent(c);
  return status;
}

