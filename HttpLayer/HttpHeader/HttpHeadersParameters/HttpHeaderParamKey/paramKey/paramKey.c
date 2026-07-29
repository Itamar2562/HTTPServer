#include "paramKey.h"
#include <stdio.h>
#include <string.h>

void initializeParamKey(paramKey *pk)
{
    pk->key=NULL;
    pk->key_value=NULL;
}


void freeParamKey(paramKey *pk){
    free(pk->key);
    free(pk->key_value);
}

int BuildParamKey(paramKey *pk,char *key, size_t keyLength, char *value, size_t valueLength)
{
    if (pk==NULL)
        return 0;
    pk->key=(char *)malloc(keyLength +1);
    pk->key_value =malloc(valueLength +1);
    if (pk->key ==NULL || pk->key_value== NULL)
    {
        freeParamKey(pk);
        fprintf(stderr, "memory error\n");
        return 0;
    }
    memcpy(pk->key, key, keyLength);
    memcpy(pk->key_value, value, valueLength);
    pk->key[keyLength]='\0';
    pk->key_value[valueLength]='\0';
    return 1;
}

void printParamKey(paramKey *pk)
{
    printf("key: %s\nvalue: %s\n",pk->key, pk->key_value );
}