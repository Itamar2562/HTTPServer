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

    addDefaultHTTPOnePointOneHeaders(response,"html" , "keep-alive" );
    return 1;
}

int  POSTwriteFile(httpResponse *response , HttpRequest *request,const char *body)
{
    char *fullPath=getCompleteFilePath(request->path , USER_FILES_PATH);
    if (fullPath ==NULL)
        return 0;

    int amountWritten = writeToFile(fullPath, body);
    if (amountWritten <=0)
        return 0;

    return BuildCreatedRresponse(response , request->version , body , amountWritten);
}

void initializeCredentials(Credentials *c){
    c->name= NULL;
    c->password = NULL;
}

void freeCredentials(Credentials *c)
{
    if (c==NULL)
        return;
    free (c->name);
    free(c->password);
    free(c);
}

Credentials *parseCredentials(const char *body)
{
    Credentials *c = (Credentials *)malloc(sizeof(Credentials));
    if (c==NULL)
        return NULL;
    initializeCredentials(c);

    size_t firstIndex = strcspn(body , "=");
    if (body[firstIndex] == '\0') //no = sign
        return NULL;
    if (strncmp (body , "username" , firstIndex) !=0)
        return NULL;
    body +=firstIndex+1;

    size_t nameLength = strcspn(body ,"&");
    if(body[nameLength] == '\0') //no & sign
        return NULL;

    c->name = (char *)malloc(nameLength +1);
    if (c->name ==NULL)
    {
        freeCredentials(c);
        return NULL;
    }
    strncpy(c->name , body , nameLength);
    c->name[nameLength] = '\0';

    body +=nameLength+1;

     size_t secondIndex = strcspn(body , "=");
    if (body[secondIndex ] == '\0') //no = sign
        return NULL;
    if (strncmp (body , "password" , secondIndex ) !=0)
        return NULL;
    body +=secondIndex +1;

    size_t passwordLength = strcspn(body ,"\0");

    c->password = (char *)malloc(passwordLength +1);
    if (c->password ==NULL)
    {
        freeCredentials(c);
        return NULL;
    }
    strncpy(c->password , body , passwordLength);
    c->password[passwordLength] = '\0';

    return c;
}


int RegisterResponse(httpResponse *response , HttpRequest *request,const char *body, sqlite3 *dbConn)
{
    Credentials *c= parseCredentials(body);
    if (c!=NULL)
    {   
        int status=1;
        if (RegisterUser(dbConn , c->name , c->password) && 
        BuildCreatedRresponse(response , request->version, c->name , strlen(c->name)))
            return 1;

        return 0;
        
    }
    freeCredentials(c);
    return 0;
}

int routePostRequest(httpResponse *response , HttpRequest *request ,const char *body , sqlite3 *dbConn )
{
    if (strcmp(request->path , "/Register") ==0 )
    {
        if (RegisterResponse(response , request ,body , dbConn))
            return 1;
            
        return 0;
    }
  
    else if (strcmp(request->path, "/Login") ==0)
        return 0;
    else
        return POSTwriteFile(response , request , body);
}   

int POSTResponse(httpResponse *response, HttpRequest *request,const char *body , sqlite3 *dbConn)
{
    if (!isPathSafe(request->path , USER_FILES_PATH))
        return 0;

    return routePostRequest(response , request , body , dbConn); 
}

