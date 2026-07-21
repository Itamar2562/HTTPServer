#pragma once

#define FILE_PATH_START "ContentLayer/content/"
#define DEFAULT_SITE "index.html"
#define SITE_ICON "home.png"

char *extractHeaderFilePath(char *header);
char *getCompleteFilePath(const char *path);
