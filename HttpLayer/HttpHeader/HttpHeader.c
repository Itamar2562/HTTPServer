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
}

void addCLRF(headerList *headerList)
{
    header *h=&headerList->headers[headerList->count-1];
    strcat(h->value,"\r\n");
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
}

int RemoveHeader(headerList *headerList, const char *key)
{
    for (int i=0; i <headerList->count; i++)
    {
        if (strcmp(headerList->headers[i].key, key)==0)
        {
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



char *buildHTTPHeadersFromHeaderList(headerList *hl, size_t *headersLength)
{
    if (hl==NULL || headersLength ==NULL)
        return NULL;
    int currSize=256;
    char *completeHeader= (char *)malloc(currSize);
    if (completeHeader ==NULL)
        return NULL;
    char *current=completeHeader;
    size_t offset=0;
    (*headersLength)=0;
    for (int i=0; i<hl->count; i++)
    {
        size_t keyLength=strlen(hl->headers[i].key);
        size_t ValueLength=strlen(hl->headers[i].value);

        if(keyLength+ValueLength>=currSize)
            {
                while (keyLength+ValueLength>=currSize)
                    currSize*=2;
                char *temp=realloc(completeHeader, currSize +1);
                if (temp==NULL)
                {
                    free(completeHeader);
                    return NULL;
                }
                completeHeader=temp;
                current = completeHeader +(*headersLength);
            }
        memcpy(current, hl->headers[i].key, keyLength);
        memcpy(current + keyLength, ": ", 2);
        offset = keyLength +2;
        (*headersLength)+=offset;
        current+=offset;

        memcpy(current, hl->headers[i].value, ValueLength);
        offset =ValueLength;
        (*headersLength)+=offset;
        current+=offset;
    }
    (*current)='\0';
    return completeHeader;
}

void printHeaders(headerList *headerList)
{
    if (headerList ==NULL)
        return;
    for (int i=0; i<headerList->count;i++)
    {   
        char *key= headerList->headers[i].key;
        char *value= headerList->headers[i].value;
        printf("key: %s\n",key);
        printf("value: %s\n",value);
    }
    printf("end\n");
}


char *skipTheRequestLine(char *headers)
{
    if (headers==NULL)
        return NULL;
    
    size_t firstLine=strcspn(headers, "\r\n");
    if (headers[firstLine]=='\0')
        return NULL;
    return headers+firstLine+2; 
}


headerList* buildHeaderListFromHTTPRequest(char *headers)
{
    if (headers==NULL)
        return NULL;
    
    int foundEnd=0;
    int error=0;

    headerList *hl=(headerList *)malloc(sizeof(headerList));
    if (hl==NULL)
        return NULL;
    initializeHeaderList(hl);

     if (strncmp(headers, "\r\n",2)==0) //no headers
        foundEnd=1;
   
    while (!foundEnd && !error)
    {   
        size_t keyLength=strcspn(headers, ":"); 

        if (headers[keyLength] == '\0') 
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
        memcpy(key, headers, keyLength);
        key[keyLength]='\0';
        headers += keyLength+1;
        while ((*headers)==' '|| (*headers)=='\t')
            headers++;
       
        size_t valueLength=strcspn(headers, "\r\n");
        //it cant be empty or end with \n or have \r without \n as well.
        if (headers[valueLength] == '\0' || headers[valueLength] == '\n' || (headers[valueLength]=='\r' && headers[valueLength+1] !='\n') ) 
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
        memcpy(value, headers, valueLength);
        value[valueLength]='\0';
        headers += valueLength+2;

        if (strncmp(headers, "\r\n",2)==0)
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


