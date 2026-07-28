#include "parameter.h"
#include <string.h>

int initializeParameter(parameter *p)
{
    p->parameter=NULL;
    p->length=0;
    int status= initializeKeyList(p->kl);
    return status;
}

void freeParameter(parameter *p)
{
    free(p->parameter);
    freeKeyList(p->kl);
}

int buildParameter(parameter *p, char *param, size_t paramLength)
{
    p->parameter=(char *)malloc(paramLength +1);
    if (p->parameter ==NULL)
        return 0;
    memcpy(p->parameter, param, paramLength);
    p->parameter[paramLength]='\0';
    p->length=paramLength;
    return 1;
}

int addParamKey(parameter *p, char *key, size_t keyLength, char *value, size_t valueLength)
{
    return addParamKey(p->kl, key, keyLength,value,valueLength );
}