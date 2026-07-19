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


const char *getStatusHeader(int statusCode)
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

void removeCLRF(headerList *headerList)
{
    header *h=&headerList->headers[headerList->count-1];
    h->value[strlen(h->value)-2]='\0';
    headerList->total_byte_length-=2;
}

void addCLRF(headerList *headerList)
{
    header *h=&headerList->headers[headerList->count-1];
    strcat(h->value,"\r\n");
    headerList->total_byte_length+=2;
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
    if (headerList->count>0)
        removeCLRF(headerList);
    header *h= &headerList->headers[headerList->count];
    
    size_t keyLength=strlen(key);
    size_t valueLength = strlen(value)+4;

    (*h).key = (char *)malloc(keyLength+1 );
    (*h).value = (char *)malloc(valueLength+1);

    if ((*h).key ==NULL || (*h).value==NULL)
        {
            perror("malloc");
            return;
        }
    strcpy((*h).key, key);
    strcpy((*h).value,value);
    strcat((*h).value,"\r\n\r\n");

    (*h).key[keyLength]='\0';
    (*h).value[valueLength]='\0';

    headerList->count++;
    headerList->total_byte_length+=keyLength+valueLength;
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
            if (i==headerList->count)
                addCLRF(headerList); // dont forget to add missing /r/n in the last place
            return 1;
        }
    }
    return 0;
}



char *buildHTTPHeaders(headerList *hl)
{
    char *completeHeader= (char *)malloc(hl->total_byte_length+1);
    completeHeader[0]='\0';
    for (int i=0; i<hl->count; i++)
    {
        strcat(completeHeader, hl->headers[i].key);
        strcat(completeHeader, hl->headers[i].value);

    }
    return completeHeader;
}

void printHeaders(headerList *headerList)
{
    printf("the total size is: %zu\n", headerList->total_byte_length);
    for (int i=0; i<headerList->count;i++)
    {   
        char *key= headerList->headers[i].key;
        char *value= headerList->headers[i].value;
        printf("key %s\n",key);
        printf("value %s\n",value);

    }
}
