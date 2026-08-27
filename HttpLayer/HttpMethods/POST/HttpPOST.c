#include "HttpPOST.h"
#include <sqlite3.h>

#include "../../../ContentLayer/ContentUtils/ContentUtils.h"


#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int BuildCreatedRresponse(httpResponse *response ,const char *version,  const char *body , int bodyLen)
{
    response->statusCode = 201;

    response->body= (char *)malloc(bodyLen +1 );

    if (body ==NULL || response->body ==NULL)
        return 0;
    memcpy(response->body, body, bodyLen);
    response->body_length=bodyLen;

    response->version= (char *)malloc(strlen(version) +1);
    if (response->version ==NULL)
        {
        free(response->body);
        return 0;
        }
    strcpy(response->version,version);

    addContentLengthHeader(response);
    return 1;
}



int POSTResponse(httpResponse *response,HttpRequest *request,const char *body)
{
     if (!isPathSafe(request->path , USER_FILES_PATH))
        return 0;
    char *fullPath=getCompleteFilePath(request->path , USER_FILES_PATH);
    if (fullPath ==NULL)
        return 0;

    int amountWritten = writeToFile(fullPath, body);
    if (amountWritten >0)
    {
        BuildCreatedRresponse(response , request->version , body , amountWritten);
        return 1;
    }
    return 0;
 
}

