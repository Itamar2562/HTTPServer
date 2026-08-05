#pragma once
#include <stdlib.h>

#define STARTING_EXTENSION_LIST_SIZE 2

enum mimeTypes
{
    TEXT,
    IMAGE,
    APPLICATION,
    UNKNOWN
};


typedef struct {
    const char *extension;
    const char *mimeType;

    enum mimeTypes mimeCategory;
} MimeTypeMap;

typedef struct {
     char **extensions;

     int length;
     int size;
}extensionList;

static const MimeTypeMap mimeTypes[] = {
    {"html", "text/html" , TEXT},
    {"htm" , "text/htm" ,TEXT} ,
    { "css" , "text/css" , TEXT} ,
    { "png",  "image/png" ,IMAGE},
    { "jpg",  "image/jpeg" ,IMAGE},
    { "jpeg", "image/jpeg" ,IMAGE},
    {"js",   "application/javascript" ,APPLICATION},
    { "json", "application/json" ,APPLICATION},
    {NULL} ,{NULL}
};



const char *getHttpMimeType(const char *fileExtention);
extensionList *getExtensionByMimeType(const char *mimeType);

int initializeExtensionList(extensionList *el);
void freeExtensionList(extensionList *el);
int addExtension(extensionList *el, const char *extension);

