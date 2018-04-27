#include <stdio.h>
#include "./sqlite/sqlite3.h"
#include <stdlib.h>
#include<string.h>
#ifndef false
 #define false 0
#endif
#ifndef true
 #define true 1
#endif

int sqliteDB_open();

int sqliteDB_close();

int sqliteDB_opt_delete(char *name);

int sqliteDB_opt_delete_by_id(char *id);

int sqliteDB_opt_add(char *name,char *id,int price);

int sqliteDB_opt_select(char *name);

int sqliteDB_opt_select_by_id(char *id);

int sqliteDB_opt_select_all();