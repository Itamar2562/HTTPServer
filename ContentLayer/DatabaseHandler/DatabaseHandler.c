#include "DatabaseHandler.h"
#include "../ContentUtils/ContentUtils.h"
#include <stdio.h>
#include <stdlib.h>

int checkSQLErrors(int rc , char *zErrMsg){
   if( rc != SQLITE_OK && rc != SQLITE_DONE)
   {
   fprintf(stderr, "SQL error: %s\n",zErrMsg != NULL ? zErrMsg : "(null)");
   sqlite3_free(zErrMsg);
   return 0;
   }     
   return 1;
}

static int callback(void *data, int argc, char **argv, char **azColName){
   fprintf(stderr, "%s: ", (const char*)data);
   
   for(int i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   
   printf("\n");
   return 0;
}

int selectAll(sqlite3 *db)
{
   const char *data = "Callback function called";

   char *sql = "SELECT * FROM Users";

   char *zErrMsg;

   int rc = sqlite3_exec(db , sql , callback , (void *)data , &zErrMsg);
   return checkSQLErrors(rc , zErrMsg);
}

int RegisterUser(sqlite3 *db , const char *name, const char *password)
{   
   int rc;
   char *zErrMsg;
   char *sql = "INSERT INTO Users (Name, Password) VALUES (?, ?);";

   sqlite3_stmt *stmt;
   rc= sqlite3_prepare_v2(db, sql, -1, &stmt, NULL); //prepares the sql text and waits for values
   if (!checkSQLErrors(rc , zErrMsg))
      return 0;

   rc = sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT); 
   if (!checkSQLErrors(rc , zErrMsg))
      return 0;

   rc = sqlite3_bind_text(stmt, 2, password, -1, SQLITE_TRANSIENT);
   if (!checkSQLErrors(rc , zErrMsg))
      return 0;

   rc = sqlite3_step(stmt); //exec with values
   if (!checkSQLErrors(rc , zErrMsg))
      return 0;

   rc = sqlite3_finalize(stmt);
   if (!checkSQLErrors(rc , zErrMsg))
         return 0;
   selectAll(db);
   return 1;
}

int createUsersTable(sqlite3 *db )
{
    char *zErrMsg;
    char *sql = "CREATE TABLE IF NOT EXISTS Users ("  \
    "Id             INTEGER PRIMARY KEY," \
    "Name           TEXT    NOT NULL," \
    "Password       TEXT     NOT NULL);";

   /* Execute SQL statement */
   int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   return checkSQLErrors(rc , zErrMsg);
}

int closeDB(sqlite3 *dbConn)
{
   return sqlite3_close(dbConn);
}

sqlite3 *openDB() {
   sqlite3 *db;
   int rc;

   const char *dbName = "database.db";
   char *fullPath = getCompleteFilePath(dbName , DATABASE_PATH);
   if (fullPath ==NULL)
      return NULL;
   rc = sqlite3_open(fullPath, &db);
   free(fullPath);
   if( rc ) 
   {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return NULL;
   }

   fprintf(stderr, "Opened database successfully\n");

   if (!createUsersTable(db))
   {
      int rc = closeDB(db);
      checkSQLErrors(rc ,NULL );
      return NULL;
   }
   return db;
   
}