#pragma once

#define FILE_PATH_START "ContentLayer/content/"
#define DEFAULT_SITE "index.html"

char *extractHeaderFilePath(char *header);
char *getCompleteFilePath(const char *path);
