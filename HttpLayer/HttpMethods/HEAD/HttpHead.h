#pragma once

#include "../GET/HttpGet.h"
#include "../../../ContentLayer/ContentLoader/ContentLoader.h"


int buildHttpHEADResponse(httpResponse *r ,Content *c , int statusCode, char *version, const char *connetion);
Content *getHeadContentBasedOnAcceptHeader(const char * filePath, HttpRequest *request);

int HEADResponse(httpResponse *response,HttpRequest *request);
