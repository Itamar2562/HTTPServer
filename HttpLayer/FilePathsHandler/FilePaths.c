#include "filePaths.h"
#include <stdlib.h>
#include <string.h>



char *extractHeaderFilePath(char *header)
{
  char * startPtr=strchr(header, '/'); //get the ptr to the first /
  if (startPtr ==NULL) // didnt find it
    return NULL;
  size_t length=strcspn(startPtr, " "); //get the length until the second space

  char *filePath= (char *)malloc(length + 1);
  if (filePath ==NULL)
    return NULL;
  strncpy(filePath, startPtr,length);
  filePath[length]='\0';

  return filePath;
}

char *getCompleteFilePath(const char *path)
{
  char *completePath= (char *)malloc(strlen(path)+ strlen(FILE_PATH_START) +1);
  if (completePath ==NULL)
    return NULL;
  strcpy(completePath, FILE_PATH_START);
  strcat(completePath, path);

  return completePath;
}