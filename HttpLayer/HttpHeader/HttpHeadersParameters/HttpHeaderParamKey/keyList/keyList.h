#pragma once
#include <stddef.h>
#include "../paramKey/paramKey.h"

#define PARAM_KEY_STARTING_SIZE 2


typedef struct {
    paramKey *keys;

    int key_count;
    int max_key_size;
} keyList;

int addParamKey(keyList *kl,char *key, size_t keyLength, char *value, size_t valueLength);
int initializeKeyList(keyList *kl);
void freeKeyList(keyList *kl);
void printParamKeys(keyList *kl);
