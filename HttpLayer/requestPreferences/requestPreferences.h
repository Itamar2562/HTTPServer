#pragma once
#include "../HttpHeader/HttpHeadersParameters/HttpHeaderParam/paramList/paramList.h"
#include "../HttpRequest/HttpRequest.h"

paramList *getAcceptMimeTypeParamList(HttpRequest *request);
paramList *getAcceptEncodingParamList(HttpRequest *request);
