#include "ContentWrite.h"
#include "../ContentUtils/ContentUtils.h"

int writeContentToFile(const char *fileName ,const char *data)
{
    FILE *f= openFile(fileName,"w");
    if (f==NULL)
        return 0;
    int status= fprintf(f, data);
    fclose(f);
    return status;
}

int addContentToFile(const char *fileName , const char *data)
{
    FILE *f= openFile(fileName,"a");
    if (f==NULL)
        return 0;
    
    int status= fprintf(f, data);
    fclose(f);
    return status;
}