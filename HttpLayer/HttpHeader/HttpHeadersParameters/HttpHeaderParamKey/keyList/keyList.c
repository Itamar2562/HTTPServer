#include "keyList.h"
#include "../paramKey/paramKey.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


void freeKeyList(keyList *kl)
{
    for (int i=0; i<kl->key_count; i++)
    {
        freeParamKey(&kl->keys[i]);
    }
    free(kl->keys);
    free(kl);
}

int initializeKeyList(keyList *kl)
{
    kl->key_count=0;
    kl->max_key_size=PARAM_KEY_STARTING_SIZE;
    kl->keys= (paramKey *)malloc(sizeof(paramKey) * kl->max_key_size);
    return kl->keys !=NULL? 1:0;
}

int addParamKeyToKeyList(keyList *kl,char *key, size_t keyLength, char *value, size_t valueLength)
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
    kl->key_count++;
    return status;
}

int addDefaultParamKey(keyList *kl)
{
   return addParamKeyToKeyList(kl,"q",1,"1.0",3);
}


void printKeyList(keyList *kl)
{
    for (int i=0; i<kl->key_count; i++)
    {
        printParamKey(&kl->keys[i]);
    }
    printf("\n");
}

double getQuilityKeyValue(keyList *kl)
{
    for (int i=0; i<kl->key_count ; i++)
    {
        if (strcmp(kl->keys[i].key,"q")==0) 
        {
            char *value = kl->keys[i].key_value;
            char *endPtr;
            double q = strtod(value, &endPtr);
            if (errno == ERANGE || endPtr == value)
                return -1;
            return q;
        }
    }
    return -1; 
}