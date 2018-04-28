//static int conut = 0;//记录打开的与数据库的连接的个数(或者连接是否存在)
#include "SQLite.h"
typedef struct listnode {
	char id[40];
	char name[40];
	int price;
	int count;
	struct listnode* next;
	struct listnode* prev;
}listnode, *pnode;
/**
**
** @auther lyj_uptech@126.com
** @date 2007-3-21
** //本程序提供了对数据库进行操作的基本接口，对于更复杂的操作，用户可以根据具体情况
**   进行修改，程序的更新，以及其他需求请联系本人或者技术支持
**   
**   例如关于主键的唯一性。用户可以适当添加功能；
**
**   对于可以访问所有table的动态sql语句可以参考其中的程序实现；
**/
sqlite3 *db;//全局的数据连接

//用户第一次打开数据库时应创建对应的table并添加适当的数据记录

static int callback(void *NotUsed, int argc, char **argv, char **azColName);
/**
* 
*建立与数据库的连接
* 
*
*/
int sqliteDB_open(){
	int rc;//操作标志
//	printf("\ntrying to open the database test.db");
	printf("\ncreat database:test.db\n\ncreat table:table merchandise(contents:id name price)\n\nand add two records\n");
	rc = sqlite3_open("test.db", &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	}
//	printf("\nopen sucess!");
/**	if(!sqliteDB_create_table())
		printf("\ntable exist");
	if(!sqliteDB_insert_records())
		printf("\ninsert table faild");
  else sqliteDB_opt_select_all();
*/	return 1;
}
/**
*
* 关闭与数据库的连接 
*
*/
int sqliteDB_close(){
	if(db != 0)  
		sqlite3_close(db);
}
/**
*
* 添加一条记录到已知或未知数据库表
*
*/
int sqliteDB_opt_add(char *name,char *id,int price){
	int rc;
	char *zErrMsg = 0;
//	char *s0=0;
//	char *s1=0;
//	char *s2=0;
	char *sql=0;//动态生成的SQL语句
	char tem_sql[256]="insert into merchandise values('";
	char tem_sql0[3] = "','";
	char tem_sql1[3] = "',";
	char tem_sql2[3] = ");";
	char tem_price[20];
	sprintf(tem_price, " %d" , price);//将int数据转换为字符串
	sql = strcat(tem_sql,name);
	sql = strcat(sql,tem_sql0);
	sql = strcat(sql,id);
	sql = strcat(sql,tem_sql1);
	sql = strcat(sql,tem_price);
	sql = strcat(sql,tem_sql2);
//	printf("\n###############################");
//	printf("\n%s",sql);
//	printf("\n###############################");
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
//	printf("\n rc = %d",rc);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}
/**
*
* 删除表中的一条记录
* @param name
*/
int sqliteDB_opt_delete(char *name){
	int rc;
	char n[4];
	char *zErrMsg = 0;
	char *sql=0;//动态生成的SQL语句
	char tem_sql[256]="delete from merchandise where name = '";//
	char tem_sql0[3] = "'";
	char tem_sql1[3] = ";";

	//删除之前查询该条记录，如存在则显示该记录，并询问是否真的要删除
	                        //如不存在则推出本次操作

	sql = strcat(tem_sql,name);
	sql = strcat(sql,tem_sql0);
	sql = strcat(sql,tem_sql1);
//	printf("\n###############################");
//	printf("\n%s",sql);
//	printf("\n###############################");
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
//	printf("\n rc = %d",rc);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}
/**
*
* 删除表中的一条记录
* @param id
*/
int sqliteDB_opt_delete_by_id(char *id){
	int rc;
	char n[4];
	char *zErrMsg = 0;
	char *sql=0;//动态生成的SQL语句
	char tem_sql[256]="delete from merchandise where id = '";//
	char tem_sql0[3] = "'";
	char tem_sql1[3] = ";";

	//删除之前查询该条记录，如存在则显示该记录，并询问是否真的要删除
	                        //如不存在则推出本次操作

	sql = strcat(tem_sql,id);
	sql = strcat(sql,tem_sql0);
	sql = strcat(sql,tem_sql1);
//	printf("\n###############################");
//	printf("\n%s",sql);
//	printf("\n###############################");
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
//	printf("\n rc = %d",rc);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}
/*******************************************************
*
* 查询符合查询条件的记录
*
*
int sqliteDB_opt_select(char *name){
	int rc;
	char *zErrMsg = 0;
	char *sql=0;//动态生成的SQL语句
	char tem_sql[256]="select * from liu where name = '";//
	char tem_sql0[3] = "'";
	char tem_sql1[3] = ";";
	sql = strcat(tem_sql,name);
	sql = strcat(sql,tem_sql0);
	sql = strcat(sql,tem_sql1);
	printf("\n###############################");
	printf("\n%s",sql);
	printf("\n###############################");
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	printf("\n rc = %d",rc);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}
/**
*
* 查询符合查询条件的记录
*
*
int sqliteDB_opt_select_all(){
	int rc;
	char *zErrMsg = 0;
	char sql[256]="select * from liu;";
	printf("\n###############################");
	printf("\n%s",sql);
	printf("\n###############################");
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	printf("\n rc = %d",rc);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}
******************************************************/
/**
*
* 修改数据库表中的记录内容
* //暂不提供修改函数2007-3-21
*/
int sqliteDB_opt_modify();
/**
*
* 用预编译的sql语句减少sql语句的分析时间
* 
* 只举例 查询语句
* @2007-3-22
* 查询语句
*/
int sqliteDB_opt_select_all(){
 sqlite3_stmt* stmt=NULL;
 char* zErrMsg=NULL;
 char *_name,*_id;
 int price=0;
 int nret=sqlite3_prepare(db,"SELECT * FROM merchandise;",strlen("SELECT * FROM merchandise;"),&stmt,(const char**)(&zErrMsg));
 if(nret!=SQLITE_OK)
  return false;
 printf("\n\tname\t\tid\t\tprice\n");
 printf("\t------------------------------------------------------------\n");
 while(1){ 
  nret=sqlite3_step(stmt);
  if(nret!=SQLITE_ROW)
   break;
  _name=sqlite3_column_text(stmt,0);
  _id=sqlite3_column_text(stmt,1);
  price=sqlite3_column_int(stmt,2);
  printf("\t%s\t\t%s\t\t%d\n",_name,_id,price);
 }
 sqlite3_finalize(stmt);
 printf("\n");
 return true;
}
/**
* 查询符合查询条件的记录
* @param name
*/
int sqliteDB_opt_select(char *name){
	sqlite3_stmt* stmt=NULL;
 	char* zErrMsg=NULL;
 	char *_name,*_id;
 	int price=0;
 	int nret=0;
	int rc;
	char *sql=0;//动态生成的SQL语句
	char tem_sql[256]="select * from merchandise where name = '";//
	char tem_sql0[3] = "'";
	char tem_sql1[3] = ";";

	sql = strcat(tem_sql,name);
	sql = strcat(sql,tem_sql0);
	sql = strcat(sql,tem_sql1);
//	printf("\n###############################");
//	printf("\n%s",sql);
//	printf("\n###############################");
	nret=sqlite3_prepare(db,sql,strlen(sql),&stmt,(const char**)(&zErrMsg));
	if(nret!=SQLITE_OK)
		return false;
 printf("\n\tname\t\tid\t\tprice\n");
 printf("\t------------------------------------------------------------\n");
 while(1){ 
  nret=sqlite3_step(stmt);
  if(nret!=SQLITE_ROW)
   break;
  _name=sqlite3_column_text(stmt,0);
  _id=sqlite3_column_text(stmt,1);
  price=sqlite3_column_int(stmt,2);
  printf("\t%s\t\t%s\t\t%d\n",_name,_id,price);
 }
	sqlite3_finalize(stmt);
	printf("\n");
	return true;
}
/**
* 查询符合查询条件的记录
* @param id
*/
int sqliteDB_opt_select_by_id(char *id){
	sqlite3_stmt* stmt=NULL;
 	char* zErrMsg=NULL;
 	char *_name,*_id;
 	int price=0;
 	int nret=0;
	int rc;
	char *sql=0;//动态生成的SQL语句
	char tem_sql[256]="select * from merchandise where id = '";//
	char tem_sql0[3] = "'";
	char tem_sql1[3] = ";";

	sql = strcat(tem_sql,id);
	sql = strcat(sql,tem_sql0);
	sql = strcat(sql,tem_sql1);
//	printf("\n###############################");
//	printf("\n%s",sql);
//	printf("\n###############################");
	nret=sqlite3_prepare(db,sql,strlen(sql),&stmt,(const char**)(&zErrMsg));
	if(nret!=SQLITE_OK)
		return false;
 printf("\n\tname\t\tid\t\tprice\n");
 printf("\t------------------------------------------------------------\n");
 while(1){ 
  nret=sqlite3_step(stmt);
  if(nret!=SQLITE_ROW)
   break;
  _name=sqlite3_column_text(stmt,0);
  _id=sqlite3_column_text(stmt,1);
  price=sqlite3_column_int(stmt,2);
  printf("\t%s\t\t%s\t\t%d\n",_name,_id,price);
 }
	sqlite3_finalize(stmt);
	printf("\n");
	return true;
}
/**
**
**
**内部调用函数
**
**/
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}
/**
**
**内部调用函数
**功能判断所查询的记录是否存在
** 依据：商品名
**/
int sqliteDB_exist(char *name){
	sqlite3_stmt* stmt=NULL;
 	char* zErrMsg=NULL;
 	char *_name;
 	int price=0;
 	int nret=0;
	int rc;
	char *sql=0;//动态生成的SQL语句
	char tem_sql[256]="select * from merchandise where name = '";//
	char tem_sql0[3] = "'";
	char tem_sql1[3] = ";";
	sql = strcat(tem_sql,name);
	sql = strcat(sql,tem_sql0);
	sql = strcat(sql,tem_sql1);
	nret=sqlite3_prepare(db,sql,strlen(sql),&stmt,(const char**)(&zErrMsg));
	if(nret!=SQLITE_OK)
		return false;
		nret=sqlite3_step(stmt);
		if(nret!=SQLITE_ROW){
			printf("\n this record does not exist!!");
			return false;
		}
	sqlite3_finalize(stmt);
	return true;
}
/**
**
**在用户还没有创建table时可用该函数创建一个table
**
**/
int sqliteDB_create_table(){
 char* zErrMsg=NULL;
 char sql[256]="CREATE TABLE merchandise(name varchar(40),id varchar(40),price smallint);";
 int nret=sqlite3_exec(db,sql,NULL,NULL,&zErrMsg);
 if(nret!=SQLITE_OK){ 
  printf("%s\n",sqlite3_errmsg(db));
  return false;
 }
 else
  return true;
}
/**
**
**初始化数据库table时预插入两条数据
**
**/
int sqliteDB_insert_records()
{
 char* zErrMsg=NULL;
 char sql1[256]="INSERT INTO merchandise VALUES('ibm','277325073',483);";
 char sql2[256]="INSERT INTO merchandise VALUES('sun','433443',400);";
 int nret=sqlite3_exec(db,sql1,NULL,NULL,&zErrMsg);
 nret=sqlite3_exec(db,sql2,NULL,NULL,&zErrMsg);
 if(nret!=SQLITE_OK){
  printf("%s\n",sqlite3_errmsg(db));
  return false;
 }
 else
  return true;
}

pnode sqliteDB_market_select_by_id(char *id) {
	sqlite3_stmt* stmt = NULL;
	char* zErrMsg = NULL;
	char *_name, *_id;
	int price = 0;
	int nret = 0;
	int rc;
	char *sql = 0;//动态生成的SQL语句
	char tem_sql[256] = "select * from merchandise where id = '";//
	char tem_sql0[3] = "'";
	char tem_sql1[3] = ";";
	pnode item = NULL;

	sql = strcat(tem_sql, id);
	sql = strcat(sql, tem_sql0);
	sql = strcat(sql, tem_sql1);
	//	printf("\n###############################");
	//	printf("\n%s",sql);
	//	printf("\n###############################");
	nret = sqlite3_prepare(db, sql, strlen(sql), &stmt, (const char**)(&zErrMsg));
	if (nret != SQLITE_OK)
		return NULL;
	// printf("\n\tname\t\tid\t\tprice\n");
	// printf("\t------------------------------------------------------------\n");
	while (1) {
		nret = sqlite3_step(stmt);
		if (nret != SQLITE_ROW)
			break;
		_name = sqlite3_column_text(stmt, 0);
		_id = sqlite3_column_text(stmt, 1);
		price = sqlite3_column_int(stmt, 2);
		// printf("\t%s\t\t%s\t\t%d\n", _name, _id, price);
		strcpy(item->id, _id);
		strcpy(item->name, _name);
		item->price = price;
		item->count = 1;
	}
	sqlite3_finalize(stmt);
	// printf("\n");
	return item;
}
