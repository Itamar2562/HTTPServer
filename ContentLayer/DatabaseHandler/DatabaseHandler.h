#pragma once
#include <sqlite3.h>

sqlite3 *openDB();
int closeDB(sqlite3 *dbConn);
int checkSQLErrors(int rc , char *zErrMsg);
int RegisterUser(sqlite3 *db , const char *name, const char *password);
