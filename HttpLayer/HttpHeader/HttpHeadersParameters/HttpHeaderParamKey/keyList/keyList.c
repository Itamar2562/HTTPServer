#include "keyList.h"
#include "../paramKey/paramKey.h"

#include <stdlib.h>
#include <stdio.h>


void freeKeyList(keyList *kl)
{
    for (int i=0; i<kl->key_count; i++)
    {
        freeParamKey(&kl->keys[i]);
    }
    free(kl->keys);
}

int initializeKeyList(keyList *kl)
{
    kl->key_count=0;
    kl->max_key_size=PARAM_KEY_STARTING_SIZE;
    kl->keys= (paramKey *)malloc(sizeof(paramKey) * kl->max_key_size);
    return kl->keys !=NULL? 1:0;
}

int addParamKey(keyList *kl,char *key, size_t keyLength, char *value, size_t valueLength)
{
    if (kl->key_count>= kl->max_key_size)
    {
        kl->max_key_size*=2;
        paramKey *temp=realloc(kl->keys,kl->max_key_size );
        if (temp==NULL)
        {
            fprintf(stderr, "memory error\n");
            return 0;
        }
        else
            kl->keys=temp;
    }

    int status = BuildParamKey(&kl->keys[kl->key_count], key, keyLength, value, valueLength);
    return status;
}

void printParamKeys(keyList *kl)
{
    for (int i=0; i<kl->key_count; i++){
        printf("key: %s\nvalue: %s\n", kl->keys[i].key, kl->keys[i].key_value);
    }
}