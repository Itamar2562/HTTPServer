#include "parameter.h"
#include <string.h>
#include <stdio.h>

int initializeParameter(parameter *p)
{
    p->parameter=NULL;
    p->value=NULL;
    p->value_length=0;
    p->param_length=0;
    p->kl = (keyList *)malloc(sizeof(keyList));
    int status= initializeKeyList(p->kl);
    return status;
}

void freeParameter(parameter *p)
{
    free(p->parameter);
    free(p->value);
    freeKeyList(p->kl);
}

int buildParameter(parameter *p, char *param, size_t paramLength, char *value, size_t value_length)
{
    p->parameter=(char *)malloc(paramLength +1);
    if (p->parameter ==NULL)
        return 0;
    memcpy(p->parameter, param, paramLength);
    p->parameter[paramLength]='\0';
    p->param_length=paramLength;

    if (value!=NULL)
    {
        p->value=(char *)malloc(value_length +1);
        if (p->value==NULL)
        {
            free(p->parameter);
            return 0;
        }
        memcpy(p->value, value, value_length);
        p->value[value_length]='\0';
        p->value_length=value_length;
    }
  
    return 1;
}

int addParamKeyToParameter(parameter *p, char *key, size_t keyLength, char *value, size_t valueLength)
{
    return addParamKeyToKeyList(p->kl, key, keyLength,value,valueLength );
}

void printParameter(parameter *p)
{
    printf("parameter: %s\n",p->parameter);
    printf("parameter value: %s\n",p->value);
    printKeyList(p->kl);
}