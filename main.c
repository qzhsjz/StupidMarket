#include "consoleshell.h"
#include "stdio.h"
#include "malloc.h"
#include "string.h"
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include "keyboard.h"
#include "item.h"
#define addItem_countplus 1
#define addItem_add 0

//��ʼ��ϵͳ�е���Ʒ���Լ��α�
char logbuffer[1000] = {0,};
char kbdbuffer[50] = { 0, };
int kbdbufcur = 0;
int delflag = 0;
int rstflag = 0;
int state = 0;
int s0editflag = 0;
pnode itemlist = malloc(sizeof(listnode));
pnode cur = itemlist;
char id[40] = { 0, };
int total = 0;
int discount = 0;
int recv = 0;
int cash = 0;

//View Layer
int viewupdate(){
  pnode p = itemlist;
  system("clear");
  printf("This is view");
  puts("---log---")
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
  printf("\t----\t--------\t--------\t--------\n"); //���Ƿָ���
  printf("\033[40;37m"); //�׵׺��ֿ�ʼ
  printf("Total Price: %d\n", total);
  if (s0editflag == 1) { //�����༭
	  printf("Enter new count for such item:%s", kbdbuffer);
  }
  if (state >= 1) { //�Ż���ʾ
	  printf("Discount: %d\n", discount);
  }
  if (state >= 2) { //������ʾ
	  printf("Recieve payment: %d\n", recv);
  }
  printf("\033[0m"); //�׵׺��ֽ���
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

//ɨ��ǹ�ļ���
int ListenBarcodeScanner(){
	int BarcodeReadCount;
	fflush(stdin);
	tty_fflush();
	fflush(stdout);
	for(i=0;i<40;i++) id[i]=0;
	BarcodeReadCount = tty_read(id, 40); // һ��Ҫ�ǵ���tty.c�и�Ϊ������ʽ���豸 �����
	for (i=0;i<40;i++) if (id[i]=='\n') id[i]=0;
	log("\nread:%d", BarcodeReadCount);
	if (BarcodeReadCount <= 0){
	log("\nread null or error happend");
	return -1;
	}
	tty_fflush();
	log("%s\n", id);
	for (i=0;i<40;i++) printf(" %X", id[i]);
	pnode p = sqliteDB_market_select_by_id(id); // �Ѹ㶨
	if (p != NULL){
	addItem(p);
	}
	return 0;
}

int ListenKeyboard(){
  char key;
  key = get_key();
  if (key == MWKEY_KP_MINUS) {
	  //�˼�����Ϊ��
	  cur = cur->prev;
  }
  else if (key == MWKEY_KP_PLUS) {
	  //�˼�����Ϊ��
	  cur = cur->next;
  }
  else if (key == MWKEY_KP_ENTER) {
	  //�˼�����Ϊ�س�ȷ��
	  if (kbdbufcur == 0) {
		  //�׶β���
		  state++;
	  }
	  else if (state == 0) {
		  if (s0editflag == 1) {
			  //����Ʒ����׶α༭����ȷ��
			  sscanf(kbdbuffer, "%d", &(cur->count));
			  strcpy(kbdbuffer, "");
			  kbdbufcur = 0;
			  s0editflag = 0;
		  }
		  else {
			  //����Ʒ����׶�ȷ������
		      strcpy(id, kbdbuffer);
		      strcpy(kbdbuffer, "");
		      kbdbufcur = 0;
		  }
	  }
	  else if (state == 1) {
		  //���Żݽ׶�ȷ���Ż�
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
  }
  else if (key == MWKEY_KP_NUMLOCK) {
	  // �˼�����Ϊ��λ
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
		  strcpy(id, "");
		  total = 0;
		  discount = 0;
		  recv = 0;
		  cash = 0;
		  // ��������
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
	  //�˼�����Ϊɾ��,�Լ��Ż�ʱ��С����
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
	  //�˼�����Ϊ�༭
	  if (state == 0) {
		  s0editflag = 1;
	  }
  }
  else if (key == MWKEY_KP0) {
	  //����0
	  kbdbuffer[kbdbufcur] = '0';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP1) {
	  //����1
	  kbdbuffer[kbdbufcur] = '1';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP2) {
	  //����2
	  kbdbuffer[kbdbufcur] = '2';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP3) {
	  //����3
	  kbdbuffer[kbdbufcur] = '3';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP4) {
	  //����4
	  kbdbuffer[kbdbufcur] = '4';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP5) {
	  //����5
	  kbdbuffer[kbdbufcur] = '5';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP6) {
	  //����6
	  kbdbuffer[kbdbufcur] = '6';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP7) {
	  //����7
	  kbdbuffer[kbdbufcur] = '7';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP8) {
	  //����8
	  kbdbuffer[kbdbufcur] = '8';
	  kbdbufcur++;
  }
  else if (key == MWKEY_KP9) {
	  //����9
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
  kbd_init();
  while(true){
    ListenBarcodeScanner();
    ListenKeyboard();
    viewupdate();
   }
  kbd_close();
  return 0;
}