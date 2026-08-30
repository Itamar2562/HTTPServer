#pragma once

#include "../../HttpResponse/HttpResponse.h"
#include "../../HttpRequest/HttpRequest.h"
#include "../../../ContentLayer/DatabaseHandler/DatabaseHandler.h"

typedef struct 
{
    char *name;
    char *password;
} Credentials;


int POSTResponse(httpResponse *response, HttpRequest *request,const char *body , sqlite3 *dbConn);
