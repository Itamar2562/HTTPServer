#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <stdlib.h>

char *buildHTTPHeaders(size_t contentLength, int statusCode);
char* loadContent(char *name);





#endif
