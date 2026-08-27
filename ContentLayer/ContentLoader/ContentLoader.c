#include <stdio.h>
#include "ContentLoader.h"
#include "../ContentUtils/ContentUtils.h"
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>

void initializeContent(Content *c)
{
    c->data=NULL;
    c->fileName=NULL;
    c->data_size=0;
    c->exists=0;
    c->type=NULL;
}



Content *loadContent(const char *fullPath)
{  
    if (fullPath == NULL)
        return NULL;
    Content *c=(Content *)malloc(sizeof(Content) );
    if (c== NULL)
        return NULL;

    initializeContent(c);        
    FILE *f=openFile(fullPath, "r");
    if (f==NULL) //file not found
        return c;

    long length=getFileSize(f);
    if (length<0 || (unsigned long)length>=SIZE_MAX )
        return NULL;

    c->data= (char *)malloc(sizeof(char)*length); 
     if (c->data==NULL)
        {
            fprintf(stderr,"memory error");
            freeContent(c);
            return NULL;
        }
    size_t bytes_read= fread(c->data,1, length, f );

    if (bytes_read<length)
    {
        fprintf(stderr , "reading file error\n");
        freeContent(c);
        return NULL;
    }
    c->type=getFileExtension(fullPath);
    c->fileName=getFileName(fullPath);

    if (c->fileName ==NULL || c->type ==NULL)
    {
        freeContent(c);
        return NULL;
    }
    
    c->data_size=bytes_read;
    c->exists=1;
    fclose(f);
    return c;
}


Content *getContentDetailsWithoutBody(const char *fullPath)
{
      if (fullPath == NULL)
        return NULL;
    Content *c=(Content *)malloc(sizeof(Content) );
    if (c== NULL)
        return NULL;

    initializeContent(c);        
    FILE *f=openFile(fullPath, "r");
    if (f==NULL) //file not found
        return c;

    long length=getFileSize(f);
    if (length<0 || (unsigned long)length>=SIZE_MAX )
        return NULL;

    c->type=getFileExtension(fullPath);
    c->fileName=getFileName(fullPath);

    if (c->fileName ==NULL || c->type ==NULL)
    {
        freeContent(c);
        return NULL;
    }
    
    c->data_size=length;
    c->exists=1;
    fclose(f);
    return c;
}

void freeContent(Content *c)
{   
    free(c->data);
    free(c->type);
    free(c->fileName);
    free(c);
}

