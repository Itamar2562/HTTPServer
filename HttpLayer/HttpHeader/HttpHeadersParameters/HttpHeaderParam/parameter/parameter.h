#pragma once
#include "../../HttpHeaderParamKey/keyList/keyList.h"
#include <stdlib.h>


typedef struct 
{
    char *parameter;
    size_t length;

    keyList *kl;
} parameter;

int initializeParameter(parameter *p);
void freeParameter(parameter *p);
int buildParameter(parameter *p, char *param, size_t paramLength);

