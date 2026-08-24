#include "requestPreferences.h"
#include <stdlib.h>

static paramList *getAcceptParamList(HttpRequest *request, const char *headerName)
{
  char *RawAcceptParameters= getHeaderValue(request->headerList, headerName);
  if (RawAcceptParameters ==NULL)
    return NULL;

  paramList *pl= parseParameterizedHeader(RawAcceptParameters);
  sortParameterizedHeaderByQuality(pl);
  free(RawAcceptParameters);
  return pl;
}



paramList *getAcceptMimeTypeParamList(HttpRequest *request)
{
  return getAcceptParamList(request, "Accept");
}

paramList *getAcceptEncodingParamList(HttpRequest *request)
{
  return getAcceptParamList(request, "Accept-Encoding");
}

