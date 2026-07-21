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



char *buildHTTPHeadersFromHeaderList(headerList *hl)
{
    char *completeHeader= (char *)malloc(hl->total_byte_length+1);
    if (completeHeader ==NULL)
        return NULL;
    char *current=completeHeader;
    for (int i=0; i<hl->count; i++)
    {
        size_t keyLength=strlen(hl->headers[i].key);
        memcpy(current, hl->headers[i].key, keyLength);
        current+=keyLength;

        size_t ValueLength=strlen(hl->headers[i].value);
        memcpy(current, hl->headers[i].value, ValueLength);
        current+=ValueLength;

    }
    (*current)='\0';
    return completeHeader;
}

void printHeaders(headerList *headerList)
{
    if (headerList ==NULL)
        return;
    printf("the total size is: %zu\n", headerList->total_byte_length);
    for (int i=0; i<headerList->count;i++)
    {   
        char *key= headerList->headers[i].key;
        char *value= headerList->headers[i].value;
        printf("key: %s\n",key);
        printf("value: %s\n",value);
    }
    printf("end\n");
}

//need to keep working on making this more defensive as this gets
//data straight from the web which could be invalid/malicious
//currently I check for maxLength to ensure it doesn't max heap and if I hit the end unexpectedly.
headerList* buildHeaderListFromHTTPRequest(char *headers)
{
    if (headers==NULL)
        return NULL;
    int foundEnd=0;
    int error=0;
    size_t maxSize =4096;
    size_t firstLine=strcspn(headers, "\r\n");
    if (headers[firstLine]=='\0')
        return NULL;
    char *start=headers+firstLine+2; //skip the first request line

    headerList *hl=(headerList *)malloc(sizeof(headerList));
    initializeHeaderList(hl);

     if (strncmp(start, "/r/n",2)==0) //no headers
        foundEnd=1;
   
    while (!foundEnd && !error)
    {   
        size_t keyLength=strcspn(start, " ")+1; //include the space

        //header execeeds max size or headers not correct (cant end on a key)
        if (keyLength>=maxSize|| start[keyLength-1] == '\0') //-1 bc we added 1 prev
        {
            error =1;
            break;
        }
        char *key=(char *)malloc(keyLength+1);
        if (key==NULL)
            {
                error=1;
                break;
            }
        memcpy(key, start, keyLength);
        key[keyLength]='\0';
        start += keyLength;

        size_t valueLength=strcspn(start, "\r\n");
        //header execeeds max size or header not correct (must end with \r\n\r\n)
        if (valueLength>=maxSize || start[valueLength] == '\0') 
        {
            free(key);
            error=1;
            break;
        }
        char *value = (char *)malloc(valueLength +1);
        if (value==NULL)
            {
                free(key);
                error=1;
                break;
            }
        memcpy(value, start, valueLength);
        value[valueLength]='\0';
        start += valueLength+2;

        if (strncmp(start, "\r\n",2)==0)
            foundEnd=1;

        addHeader(hl, key, value);
        free(key);
        free(value);
    }
    if (error)
    {
        freeHeaderList(hl);
        return NULL;
    }  
    return hl;
}


char *findHeaderValue(headerList *hl, char *key)
{
     for (int i=0; i <hl->count; i++)
    {
        if (strcmp(hl->headers[i].key, key)==0)
        {
            return hl->headers[i].value;
        }
    }
    return NULL;

}