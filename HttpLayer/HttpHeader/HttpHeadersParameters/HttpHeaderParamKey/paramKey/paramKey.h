#pragma once
#include <stdlib.h>

typedef struct {

    char *key;
    char *key_value;
    
} paramKey;

void initializeParamKey(paramKey *pk);
void freeParamKey(paramKey *pk);
int *BuildParamKey(paramKey *pk,char *key, size_t keyLength, char *value, size_t valueLength);
