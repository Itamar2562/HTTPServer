#include "requestPreferences.h"
#include <stdlib.h>


paramList *getAcceptMimeTypeParamList(HttpRequest *request)
{
    char *RawAcceptParameters= getHeaderValue(request->headerList, "Accept");
    if (RawAcceptParameters ==NULL)
      return NULL;

    paramList *pl= parseParameterizedHeader(RawAcceptParameters);
    if (pl==NULL)
      return NULL;
    sortParameterizedHeaderByQuality(pl);
    return pl;
}

paramList *getAcceptEncodingParamList(HttpRequest *request)
{
  char *RawAcceptParameters= getHeaderValue(request->headerList, "Accept-Encoding");
  if (RawAcceptParameters ==NULL)
    return NULL;

  paramList *pl= parseParameterizedHeader(RawAcceptParameters);
  sortParameterizedHeaderByQuality(pl);
  return pl;
}