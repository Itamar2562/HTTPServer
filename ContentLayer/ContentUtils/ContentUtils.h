#pragma once
#include <stdio.h>

#define FILE_PATH_START "ContentLayer/content/Files/ServerFiles/"
#define USER_FILES_PATH_START "ContentLayer/content/Files/UserFiles/"

typedef enum 
{
    DEFAULT_PATH,
    USER_FILES_PATH
} paths;

int isPathSafe(const char *userFilepath , paths p );
char *getCompleteFilePath(const char *path , paths p);
FILE *openFile(const char *filePath ,const char *mode );
char *getFileExtension(const char *filepath);
int hasFileExtension(const char *filePath);
char *changeFileExtension(const char *filePath, const char *extension);
char *getFileName(const char *filePath);
long getFileSize(FILE *f);
char *omitFileName(const char *filePath);
int writeToFile(const char *filePath , const char *data);



