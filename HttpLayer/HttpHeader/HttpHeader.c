#include "HttpHeader.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

void freeHeader(header *h)
{
    free(h->key);
    free(h->value);
}

void freeHeaderList(headerList *hl)
{
    for (int i=0; i<hl->count; i++)
    {
        freeHeader(&hl->headers[i]);
    }
    free(hl);
}

int initializeHeaderList(headerList *h)
{
    h->count=0;
    h->max_size=MAX_HEADER_COUNT;
    h->total_byte_length=0;
    h->headers=(header* )malloc(sizeof(header)*h->max_size);
    return  h->headers!=NULL ?  1 : 0;
}


char *getStatusHeader(int statusCode)
{
    char *status;
    switch(statusCode)
    {
        case 200:
            status="200 OK";
            break;
        case 404:
            status="404 Not Found";
            break;
        default:
            status= "500 Internal Server Error";
            break;
    }
    return status;
}


void addHeader(headerList *headerList, const char *key, const char *value)
{
    if (headerList->count>=headerList->max_size)
    {
        headerList->max_size *=2;
        header *temp = realloc(headerList->headers, sizeof(header)* headerList->max_size );
        if (temp==NULL)
        {
            perror ("malloc");
            return;
        }
        headerList->headers=temp;
    }

    header *h= &headerList->headers[headerList->count];
    
    size_t keyLength=strlen(key);
    size_t valueLength = strlen(value);

    (*h).key = (char *)malloc(keyLength+1 );
    (*h).value = (char *)malloc(valueLength+1);

    if ((*h).key ==NULL || (*h).value==NULL)
        {
            perror("malloc");
            return;
        }
    strcpy((*h).key, key);
    strcpy((*h).value,value);

    (*h).key[keyLength]='\0';
    (*h).value[valueLength]='\0';

    headerList->count++;
    headerList->total_byte_length+=strlen(key)+strlen(value);
}

int RemoveHeader(headerList *headerList, const char *key)
{
    for (int i=0; i <headerList->count; i++)
    {
        if (strcmp(headerList->headers[i].key, key)==0)
        {
            headerList->total_byte_length-=strlen(headerList->headers[i].key)+strlen(headerList->headers[i].value);
            freeHeader(&headerList->headers[i]);
            headerList->headers[i]=headerList->headers[headerList->count-1];
            headerList->count--;
            return 1;
        }
    }
    return 0;
}



char *buildHTTPHeaders(size_t contentLength, int statusCode)
{
    char *status=getStatusHeader(statusCode);

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

void printHeaders(headerList *headerList)
{
    printf("the total size is: %zu\n", headerList->total_byte_length);
    for (int i=0; i<headerList->count;i++)
    {
        printf("key: %s\nvalue: %s\n", headerList->headers[i].key, headerList->headers[i].value);
    }
}
