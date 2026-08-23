#include "MimeTypes.h"
#include <string.h>

int initializeExtensionList(extensionList *el)
{
  el->size=STARTING_EXTENSION_LIST_SIZE;
  el->extensions= (char **)malloc(sizeof(char *) * STARTING_EXTENSION_LIST_SIZE);
  if (el->extensions ==NULL)
    return 0;
  el->length=0;
  return 1;
}

void freeExtensionList(extensionList *el)
{
  if (el==NULL)
    return;
  for (int i=0; i<el->length; i++)
    free(el->extensions[i]);

  free(el->extensions);
  free(el);
}

int addExtension(extensionList *el, const char *extension)
{
  if (el==NULL)
    return 0;
  if (el->length>=el->size)
  {
    el->size *=2;
    char **temp = realloc(el->extensions, sizeof(char *)*el->size);
    if (temp==NULL)
      return 0;
    el->extensions=temp;
  }

  el->extensions[el->length]= malloc(strlen(extension) +1);
  if (el->extensions[el->length]==NULL)
    return 0;
  strcpy(el->extensions[el->length++],extension);
  return 1;
}


const char *getHttpMimeType(const char *fileExtention)
{
  for (int i=0; mimeTypes[i].mimeType!=NULL; i++)
  {
    if (strcmp(fileExtention, mimeTypes[i].extension)==0)
        return mimeTypes[i].mimeType;
  }
   return "application/octet-stream"; //default bytes
}

int buildExtensionListAll(extensionList *el){
  
  for (int i=0; mimeTypes[i].mimeType !=NULL; i++)
    addExtension(el, mimeTypes[i].extension);
  return 1;
}

static enum mimeTypes routeMimeTypes(const char *mimeType)
{
  if (strcmp(mimeType,"image")==0)
    return IMAGE;
  else if (strcmp(mimeType, "text")==0)
    return TEXT;
  else if (strcmp(mimeType, "application")==0)
    return APPLICATION;

  return UNKNOWN;
}

int buildWildCardExtensionList(extensionList *el ,const char *wildCard)
{
  enum mimeTypes type=routeMimeTypes(wildCard);
  if (type ==UNKNOWN)
    return 0;
  for (int i=0; mimeTypes[i].extension !=NULL; i++)
  {
    if (mimeTypes[i].mimeCategory==type)
        addExtension(el,mimeTypes[i].extension);
  }
  return 1;
}



extensionList *getExtensionByMimeType(const char *mimeType)
{ 
  extensionList *el = (extensionList *)malloc(sizeof(extensionList));
  if (el==NULL)
    return NULL;
  initializeExtensionList(el);

  if (strcmp(mimeType, "*/*")==0)
  {
      buildExtensionListAll(el);
      return el;
  }
  
  size_t typeLength=strcspn(mimeType, "/");
  if (mimeType[typeLength]!='\0' && mimeType[typeLength+1]=='*')
  {
    char wildCard[typeLength+1];
    memcpy(wildCard, mimeType, typeLength);
    wildCard[typeLength]='\0';
    int status = buildWildCardExtensionList(el, wildCard);
    if (!status)
      return NULL;
    return el;
  }

  for (int i=0; mimeTypes[i].mimeType!=NULL; i++)
  {
    if (strcmp(mimeType, mimeTypes[i].mimeType)==0)
        addExtension(el,mimeTypes[i].extension);
  }
   return el;
}


