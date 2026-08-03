#include "paramList.h"
#include <string.h>
#include <stdio.h>

int initializeParamList(paramList *pl)
{
    pl->param_count=0;
    pl->param_max_size=STARTING_MAX_PARAM_SIZE;
    pl->parameters= (parameter *)malloc(sizeof(parameter) * pl->param_max_size);
    for (int i=0; i<pl->param_max_size; i++)
    {
        initializeParameter(&pl->parameters[i]);
    }
    return 1;
}

void freeParamList(paramList *pl)
{
    for (int i=0; i<pl->param_count; i++){
        freeParameter(&pl->parameters[i]);
    }
    free(pl->parameters);
    free(pl);
}

int addParam(paramList *pl, char *param, size_t paramLength, char *value, size_t valueLength)
{
    if (pl->param_count>=pl->param_max_size)
    {
        pl->param_max_size *=2;
        parameter *temp= realloc(pl->parameters, sizeof(parameter)* pl->param_max_size);
        if (temp==NULL)
            return 0;
        {
            pl->parameters=temp;
            for (int i=pl->param_max_size/2; i<pl->param_max_size; i++){
                initializeParameter(&pl->parameters[i]);
            }
        }
    }
    int status= buildParameter(&pl->parameters[pl->param_count], param, paramLength,value,valueLength);
    if (status ==0) 
        return status;
    pl->param_count++;
    return 1;
}

void printParameterList(paramList *pl)
{
    for (int i=0; i<pl->param_count; i++)
    {
        printParameter(&pl->parameters[i]);
    }
    printf("\n");
}

size_t getParamLength(char **curr)
{
    size_t paramLength=0;
    while(**curr!=';'&& **curr!=',' && **curr!='=' && **curr !='\r')
        if(**curr!='"')
        {
            (*curr)++;
            paramLength++;
        }
        else
            (*curr)++;
    return paramLength;
}

size_t getParamValueLength(char **curr)
{
    size_t valueLength=0;
    if (**curr=='=')
        {
            (*curr)++;
            while(**curr!=';'&& **curr !=',' && **curr!='\r')
            {
                (*curr)++;
                valueLength++;
            }
        }
    return valueLength;
}

size_t getParamKeyLength(char **curr)
{
    size_t keyLength=0;
    while(**curr!='=')
        if(**curr!='"')
        {
            (*curr)++;
            keyLength++;
        }
        else
            (*curr)++;
    return keyLength;
}

size_t getParamKeyValueLength(char **curr)
{
    size_t valueLength=0;
     while(**curr!=';' && **curr!=',' && **curr !='\r')
        if(**curr!='"')
        {
            (*curr)++;
            valueLength++;
        }
        else
            (*curr)++;
    return valueLength;
}

paramList *parseParameterizedHeader(char *value)
{
    paramList *pl= (paramList *)malloc(sizeof(paramList));
    if (pl==NULL)
        return NULL;
    initializeParamList(pl);

    char *curr=value;

    while(*curr!='\r')
    {
      while(*curr==',' || *curr==' ')
        curr++;
      while(*curr!=',' && *curr!='\r')
      {
        while (*curr==' ')
            curr++;
        char *param=curr;
        if (*param=='"')
            {
                param++;
                curr++;
            }
        size_t paramLength=getParamLength(&curr);

        char *paramValue=curr+1;
        size_t valueLength=getParamValueLength(&curr);
        if (valueLength==0)
            paramValue=NULL;

        addParam(pl, param, paramLength,paramValue,valueLength);
        if (*curr=='\r')
            continue;
        if (*curr==',')
        {
            addDefaultParamKey(pl->parameters[pl->param_count-1].kl);
            curr++; 
            continue;
        }
        while(*curr!=','&& *curr!='\r')
        {
            curr++;
            char *key=curr;
            if(*key=='"')
                key++;
            size_t keyLength=getParamKeyLength(&curr);
            curr++; 

            char *key_value=curr;
            if (*key_value=='"')
                key_value++;
            size_t valueLength=getParamKeyValueLength(&curr);
            addParamKeyToParameter(&pl->parameters[pl->param_count-1], key,keyLength, key_value, valueLength);
        }
      }  

    }
    return pl;
}

void mergeSortRecursion(paramList *pl, int l, int r)
{
    if (l<r)
    {
         int m = l+ (r-l)/2;

        mergeSortRecursion(pl,l,m);
        mergeSortRecursion(pl, m+1 , r);

        int left_length= m-l+1;
        int right_length = r-m;

        parameter temp_left[left_length];
        parameter temp_right[right_length];

        for (int i=0; i<left_length; i++)
            temp_left[i]=pl->parameters[l+i];

        for (int i=0; i<right_length; i++)
            temp_right[i]=pl->parameters[m +1 +i];
        
        int i,j,k;
        for (i=0, j=0, k=l; k<=r; k++)
        {
            if ((i<left_length) && (j>=right_length || (getQuilityKeyValue(temp_left[i].kl) >= getQuilityKeyValue(temp_right[j].kl) )) )
            {
                pl->parameters[k]=temp_left[i];
                i++;
            }
            else{
                pl->parameters[k]=temp_right[j];
                j++;
            }
        }
    }
   
}

void sortParameterizedHeaderByQuality(paramList *pl)
{
  mergeSortRecursion(pl, 0, pl->param_count -1);   
}

