#include "consoleshell.h"
char shell_s[]="\nshell> ";
/**
**
**控制台
**
*/
void get_line(char *cmd);
void * consoleshell(){
	int count=0;
	int i;
	char cmd[256]={0,};	
//char cmd_sub[20]={0,};	
	int rc = sqliteDB_open();
  char name[40],id[40];
  int price = 0;//demo中只做整型的，浮点型的转换可以参考网上实现
	
	printf("\n<miniprint control shell>");
	printf("\n [1]  select all the records in table merchandise");
	printf("\n [2]  select the the record which you known its name");
	printf("\n [3]  select the record by the BarCode Scanner");
	printf("\n [4]  delete record");
	printf("\n [5]  add record");
	printf("\n [**] help menu");
	printf("\n [0]  print the end of the context an exit");
	while(1){
		printf(shell_s);
		fflush(stdout);
		get_line(cmd);
//		printf("\r\nwe get it!!!!!!!!!!!!");				//显示必要的输出

		if(strncmp("1",cmd,1)==0){
				sqliteDB_opt_select_all();
		}else if(strncmp("2",cmd,1)==0){
				printf("\nenter the record name ");
	  		printf("\nname:");
	  		scanf("%s",name);
	  		fflush(stdin);//刷新缓冲区
//	  		printf("get: %s",name);
	  		if(!sqliteDB_exist(name))
						continue;
				sqliteDB_opt_select(name);
		}else if(strncmp("3",cmd,1)==0){
	  		fflush(stdin);//刷新缓冲区
	  		tty_fflush();//2007-3-23刷新缓存
	  		printf("\nenter the id:");
	  		fflush(stdout);
	  		count = tty_read(id,40);
	  		printf("\nread:%d   ",count);
				if(count <= 0){
						printf("\nread null or error happened");
						continue;
				}
				tty_fflush();//2007-3-23
				printf("%s\n",id);
				for(i=0;i<40;i++)
				   printf("%x",id[i]);
				sqliteDB_opt_select_by_id(id);
		}else if(strncmp("4",cmd,1)==0){
				printf("\nplease enter the info of the record you want to delete!");
	  		printf("\nname:");
	  		scanf("%s",name);
	  		fflush(stdin);//刷新缓冲区
//	  		printf("get: %s",name);
	  		if(!sqliteDB_exist(name))
						continue;
				sqliteDB_opt_select(name);
				//删除之前应再次询问是否删除，不可回滚
				sqliteDB_opt_delete(name);
		}else if(strncmp("5",cmd,1)==0){
	  		printf("\nplease enter the info of the record you want to add!\nid:");
	  //		fflush(stdout);
				if(tty_read(id,40) <= 0){
						printf("\nread null or error happened");
						continue;
				}
				printf("%s",id);
				//tty_fflush();
	  		//刷新缓冲区
	  	
			printf("get: %s",id);
	  		printf("\nname:");
			 //fflush(stdout);
	  		scanf("%s",name);
	  		//fflush(stdin);//刷新缓冲区
//	  		printf("get: %s",name);
	  		printf("\nprice:");
	  		scanf("%d",&price);
	  		//fflush(stdin);//刷新缓冲区
//	  		printf("get%d",price);  		
	  		sqliteDB_opt_add(name,id,price);
			fflush(stdin);
			fflush(stdout);
		}else if(strncmp("**",cmd,2)==0){
				printf("\n<miniprint control shell>");
				printf("\n [1]  select all the records in table merchandise");
				printf("\n [2]  select the the record which you known its name");
				printf("\n [3]  select the record by the BarCode Scanner");
				printf("\n [4]  delete record");
				printf("\n [5]  add record");
				printf("\n [**] help menu");
				printf("\n [0]  print the end of the context an exit");
		}else if(strncmp("0",cmd,1)==0){
				sqliteDB_close();//关闭数据库连接
				break;
		}else if(cmd[0] != "0"){
			 system(cmd);
		}
	}
}
void get_line(char *cmd){
	int i=0;
	while(1){
		cmd[i]=getchar();
		if(cmd[i]==10){
			cmd[i]=0;
			break;
		}
		fflush(stdout);
		i++;
	}
}

