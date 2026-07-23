#include "MimeTypes.h"
#include <string.h>


const char *getHttpMimeType(const char *fileExtention)
{
  for (int i=0; mimeTypes[i].mimeType!=NULL; i++)
  {
    if (strcmp(fileExtention, mimeTypes[i].extension)==0)
        return mimeTypes[i].mimeType;
  }
   return "application/octet-stream"; //default bytes
}

const char *getExtensionByMimeType(const char *mimeType)
{
  for (int i=0; mimeTypes[i].mimeType!=NULL; i++)
  {
    if (strcmp(mimeType, mimeTypes[i].mimeType)==0)
        return mimeTypes[i].extension;
  }
   return NULL;
}
