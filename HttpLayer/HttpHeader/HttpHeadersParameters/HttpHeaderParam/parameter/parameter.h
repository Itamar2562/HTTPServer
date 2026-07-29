#pragma once
#include "../../HttpHeaderParamKey/keyList/keyList.h"
#include <stdlib.h>


typedef struct 
{
    char *parameter;
    size_t param_length;
    char *value;
    size_t value_length;

    keyList *kl;
} parameter;

int initializeParameter(parameter *p);
void freeParameter(parameter *p);
int buildParameter(parameter *p, char *param, size_t paramLength, char *value, size_t value_length);
int addParamKeyToParameter(parameter *p, char *key, size_t keyLength, char *value, size_t valueLength);
void printParameter(parameter *p);

