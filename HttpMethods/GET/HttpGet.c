#include "HttpGet.h"
#include "HttpResponse/HttpResponse.h"
#include "../../ContentLayer/content/NotFound.html"

char *RouteGetRequests(char *headers)
{

  //char *name=getFile name by request header
    //content type = getType By file type
    //go to build response with the correct data
  
}

void buildGetResponse(httpResponse *response,const char *name, const char *contentType)
{
  response->body=loadContent(name);
  if (response->body==NULL)
  {
    response->statusCode=404;
    //get 404 body and len
  }

  else
     response->statusCode=200;
  
   response->body_length=strlen(response->body); //we return an error strlen on NULL TEMP!!!
  char buffer[256];
  snprintf(buffer, sizeof(buffer), "%zu", response->body_length);

  addHeader(response->headers, "HTTP/1.1 ", getStatusHeader( response->statusCode));
  addHeader(response->headers,"Content-Length",buffer );
  addHeader(response->headers,"Content-Type",contentType);
  addHeader(response->headers, "Connection", "keep-alive");
}
