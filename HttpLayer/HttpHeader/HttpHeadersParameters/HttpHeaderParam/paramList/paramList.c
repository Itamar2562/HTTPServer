#include "paramList.h"


int initializeParamList(paramList *pl)
{
    initializeParameter(pl->p);
    pl->param_count=0;
    pl->param_max_size=STARTING_MAX_PARAM_SIZE;
    return 1;
}

void freeParamList(paramList *pl)
{
    for (int i=0; i<pl->param_count; i++){
        freeParameter(&pl->p[i]);
    }
    free(pl->p);
}

paramList *parseParameters(char *value)
{
    parameter *pl= (paramList *)malloc(sizeof(paramList));
    if (pl==NULL)
        return NULL;
    size_t offset=0;
    char *current=value;
    
    while(current[offset]!='\0')
    {
        while(current[offset]==' '|| current[offset]=="\"") 
            offset++;

        current +=offset;
        size_t blocklength=strcspn(current,",");

        for (size_t i=0; i<blocklength;)
        {
            size_t keyLength=strcspn(current, ";");
            if (keyLength<blocklength) //found key
            {
                if (strstr(current, "Not") && strstr(current, "Brand"))
                    break;

                
                
            }

        }
        offset= blocklength+1;

    }
}