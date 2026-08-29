#pragma once

#include "../../HttpResponse/HttpResponse.h"
#include "../../HttpRequest/HttpRequest.h"

int POSTResponse(httpResponse *response, HttpRequest *request,const char *body , const char *IP);
