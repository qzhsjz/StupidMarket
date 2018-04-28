#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#define addItem_countplus 1
#define addItem_add 0
#define MWKEY_KP_PLUS 43
#define MWKEY_KP_MINUS 45
#define MWKEY_KP_NUMLOCK 0
#define MWKEY_KP_MULTIPLY 42
#define MWKEY_KP_ENTER 13
#define MWKEY_KP_DEL 46
#define MWKEY_KP0 48
#define MWKEY_KP1 49
#define MWKEY_KP2 50
#define MWKEY_KP3 51
#define MWKEY_KP4 52
#define MWKEY_KP5 53
#define MWKEY_KP6 54
#define MWKEY_KP7 55
#define MWKEY_KP8 56
#define MWKEY_KP9 57
#define TTY_PATH            "/dev/tty"
#define STTY_US             "stty raw -echo -F "
#define STTY_DEF            "stty -raw echo -F "

static int get_char();

typedef struct listnode {
	char id[40];
	char name[40];
	int price;
	int count;
	struct listnode* next;
	struct listnode* prev;
}listnode, *pnode;


//测试函数
char get_key(){
    return get_char();
}

static int get_char()
{
    fd_set rfds;
    struct timeval tv;
    int ch = 0;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 10; //设置等待超时时间

    //检测键盘是否有输入
    if (select(1, &rfds, NULL, NULL, &tv) > 0)
    {
        ch = getchar(); 
    }

    return ch;
}

listnode test[10];
int testinit(){
    int i;
    for(i=0;i<10;i++){
        char no[10] = "test";
        char idno[40] = "000000102580";
        char buf[2];
        sprintf(buf, "%d", i);
        strcat(no, buf);
        strcat(idno, buf);
        strcat(idno, buf);
        strcat(idno, buf);
        strcat(idno, buf);
        strcpy(test[i].name, no);
        strcpy(test[i].id, idno);
    }
}

pnode sqliteDB_market_select_by_id(id){
    return &test[id];
}



//初始化系统中的商品表以及游标
char logbuffer[1000] = {0,};
char kbdbuffer[50] = { 0, };
int kbdbufcur = 0;
int delflag = 0;
int rstflag = 0;
int state = 0;
int s0editflag = 0;
int s1editflag = 0;
int s2editflag = 0;
pnode itemlist, cur;
char id[40] = { 0, };
int total = 0;
int discount = 0;
int recv = 0;
int cash = 0;

int init(){
    itemlist = malloc(sizeof(listnode));
    cur = itemlist;
}

//View Layer
int viewupdate(){
  pnode p = itemlist;
  system("clear");
  printf("This is view");
  puts("---log---");
  puts(logbuffer);
  putchar('\n');
  printf("\t----\t--------\t--------\t--------\n");
  printf("\t----\tname\tprice\tcount\n");
  printf("\t----\t--------\t--------\t--------\n");
  while(p->next != NULL){
    p = p->next;
    if (p == cur){
      printf("\t -> ");
    }
    else{
      printf("\t");
    }
    printf("\t%s\t%d.%d\t%d\n", p->name, p->price/100, p->price%100, p->count);
  }
  printf("\t----\t--------\t--------\t--------\n"); //我是分割线
  printf("\033[40;37m"); //白底黑字开始
  printf("Total Price: %d\n", total);
  if (s0editflag == 1) { //数量编辑
	  printf("Enter new count for such item:%s", kbdbuffer);
  }
  if (state >= 1) { //优惠显示
	  if (s1editflag) {
		  printf("(Please input)\n");
	  }
	  printf("Discount: %d\n", discount);
  }
  if (state >= 2) { //货款显示
	  printf("Recieve payment: %d\n", recv);
  }
  printf("\033[0m"); //白底黑字结束
  if (delflag == 1) {
	  printf("\n\t\033[31;43;5mPress DEL again to delete the item.");
  }
  if (rstflag == 1) {
	  printf("\n\t\033[31;43;5mPress NUMLOCK again to reset.");
  }
}

void log(char* content){
  strcat(logbuffer, content);
}

//扫描枪的监听
int ListenBarcodeScanner(){
	int BarcodeReadCount, i;
	fflush(stdin);
	tty_fflush();
	fflush(stdout);
	for(i=0;i<40;i++) id[i]=0;
	BarcodeReadCount = tty_read(id, 40); // 一定要记得在tty.c中改为非阻塞式打开设备 已完成
	for (i=0;i<40;i++) if (id[i]=='\n') id[i]=0;
	//log("\nread:%d", BarcodeReadCount);
	if (BarcodeReadCount <= 0){
	//log("\nread null or error happend");
	return -1;
	}
	tty_fflush();
	//log("%s\n", id);
	for (i=0;i<40;i++) printf(" %X", id[i]);
	pnode p = sqliteDB_market_select_by_id(id); // 已搞定
	if (p != NULL){
	addItem(p);
	}
	return 0;
}

int ListenKeyboard(){
  char key;
  key = get_key();
  printf("%d", key);
  if (key == MWKEY_KP_MINUS) {
	  //此键定义为↑
	  cur = cur->prev;
  }
  else if (key == MWKEY_KP_PLUS) {
	  //此键定义为↓
	  cur = cur->next;
  }
  else if (key == MWKEY_KP_ENTER) {
	  //此键定义为回车确定
	  if (kbdbufcur == 0) {
		  //阶段步进
		  state++;
	  }
	  else if (state == 0) {
		  if (s0editflag == 1) {
			  //在商品输入阶段编辑数量确定
			  sscanf(kbdbuffer, "%d", &(cur->count));
			  strcpy(kbdbuffer, "");
			  kbdbufcur = 0;
			  s0editflag = 0;
		  }
		  else {
			  //在商品输入阶段确定输入
		      strcpy(id, kbdbuffer);
		      strcpy(kbdbuffer, "");
		      kbdbufcur = 0;
		  }
	  }
	  else if (state == 1) {
		  //在优惠阶段确定优惠
		  int a, b;
		  char *as, *bs;
		  as = strtok(kbdbuffer, ".");
		  bs = strtok(NULL, ".");
		  if (strlen(bs) == 1) {
			  strcat(bs, "0");
		  }
		  sscanf(as, "%d", &a);
		  sscanf(bs, "%d", &b);
		  discount = a * 100 + b;
		  strcpy(kbdbuffer, "");
		  kbdbufcur = 0;
	  }
	  else if (state == 2) {
		  //在支付阶段输入货款
		  int a, b;
		  char *as, *bs;
		  as = strtok(kbdbuffer, ".");
		  bs = strtok(NULL, ".");
		  if (strlen(bs) == 1) {
			  strcat(bs, "0");
		  }
		  sscanf(as, "%d", &a);
		  sscanf(bs, "%d", &b);
		  recv = a * 100 + b;
		  cash = total - recv;
		  strcpy(kbdbuffer, "");
		  kbdbufcur = 0;
	  }
  }
  else if (key == MWKEY_KP_NUMLOCK) {
	  // 此键定义为复位
	  if (rstflag == 0) {
		  rstflag++;
	  }
	  else if (rstflag == 1) {
		  strcpy(logbuffer, "");
		  strcpy(kbdbuffer, "");
		  kbdbufcur = 0;
		  delflag = 0;
		  rstflag = 0;
		  state = 0;
		  s0editflag = 0;
		  s1editflag = 0;
		  s2editflag = 0;
		  strcpy(id, "");
		  total = 0;
		  discount = 0;
		  recv = 0;
		  cash = 0;
		  // 重置链表
		  cur = itemlist;
		  pnode p = itemlist->next;
		  while (p->next != NULL) {
			  p = p->next;
			  free(p->prev);
		  }
		  free(p);
	  }
  }
  else if (key == MWKEY_KP_DEL) {
	  //此键定义为删除,以及优惠时的小数点
	  if (state == 0) {
		if (delflag == 0) {
			delflag++;
		}
		else if (delflag == 1) {
			delItem(cur);
			delflag = 0;
		}
	  }
	  else if (state == 1) {
		  kbdbuffer[kbdbufcur] = '.';
		  kbdbufcur++;
	  }
  }
  else if (key == MWKEY_KP_MULTIPLY) {
	  //此键定义为编辑
	  if (state == 0) {
		  s0editflag = 1;
	  }
	  else if (state == 1) {
		  s1editflag = 1;
	  }
	  else if (state == 2) {
		  s2editflag = 1;
	  }
  }
  else if (key == MWKEY_KP0) {
	  //按键0
	  kbdbuffer[kbdbufcur] = '0';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP1) {
	  //按键1
	  kbdbuffer[kbdbufcur] = '1';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP2) {
	  //按键2
	  kbdbuffer[kbdbufcur] = '2';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP3) {
	  //按键3
	  kbdbuffer[kbdbufcur] = '3';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP4) {
	  //按键4
	  kbdbuffer[kbdbufcur] = '4';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP5) {
	  //按键5
	  kbdbuffer[kbdbufcur] = '5';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP6) {
	  //按键6
	  kbdbuffer[kbdbufcur] = '6';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP7) {
	  //按键7
	  kbdbuffer[kbdbufcur] = '7';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP8) {
	  //按键8
	  kbdbuffer[kbdbufcur] = '8';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP9) {
	  //按键9
	  kbdbuffer[kbdbufcur] = '9';
	  kbdbufcur++;
  }
}

int addItem(pnode item){
  pnode p = itemlist;
  while(p->next != NULL){
    p = p->next;
    if(p->id == item->id){
      p->count++;
	  total += item->price;
      return addItem_countplus;
    }
  }
  item->next = cur->next;
  cur->next = item;
  item->prev = cur;
  item->next->prev = item;
  cur = item;
  total += item->price;
  return addItem_add;
}

int delItem(pnode item) {
	item->prev->next = item->next;
	item->next->prev = item->prev;
	total = total - (item->price * item->count);
	free(item);
	return 0;
}


int main(){
    init();
    testinit();
  // kbd_init();
  while(1){
    //ListenBarcodeScanner();
    ListenKeyboard();
    viewupdate();
   }
  // kbd_close();
  return 0;
}
