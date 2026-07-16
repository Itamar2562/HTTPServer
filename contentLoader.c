#include <stdio.h>
#include "serverUtils.h"


char *buildHTTPHeaders(size_t contentLength, int statusCode)
{
    char *status;
    switch (statusCode)
    {
    case 200:
        status="200 OK";
        break;
    default:
        break;
    }

    int bytesNeeded= snprintf(NULL,0,
    "HTTP/1.1 %s\r\n"
    "Content-Length: %zu\r\n"
    "Content-Type: text/html\r\n"
    "Connection: keep-alive\r\n\r\n",status,contentLength);

    char *headerBuffer = (char *)malloc(sizeof(char) *bytesNeeded+1);
    if (headerBuffer==NULL)
        {
            printf("header buffer memory error");
            return NULL;
        }
    snprintf(headerBuffer,bytesNeeded+1,
    "HTTP/1.1 %s\r\n"
    "Content-Length: %zu\r\n"
    "Content-Type: text/html\r\n"
    "Connection: keep-alive\r\n\r\n",status,contentLength);
    return headerBuffer;
}

char* loadContent(char *name)
{   
    int startingLength=10;
    FILE *f=fopen(name,"r");

    if (f==NULL){
    printf("file not found");
        return NULL;
    }
     

    if (fseek(f,0,SEEK_END) ==-1) //find the offset 
        perror("fseek end");
    long length=ftell(f);
    if (length==-1){
        perror("ftell");
        return NULL;
    }

    if (fseek (f,0,SEEK_SET)==-1)
        perror("fseek start");

    char *buffer= (char *)malloc(sizeof(char)*length +1); //+1 for null \0
     if (buffer==NULL)
        {
            printf("body buffer memory error");
            return NULL;
        }
    size_t read_bytes= fread(buffer, sizeof(char), length, f);
    buffer[read_bytes]='\0'; // make it a c string
    fclose(f);
    return buffer;
}
