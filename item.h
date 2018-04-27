typedef struct listnode {
	char id[40];
	char name[40];
	int price;
	int count;
	struct listnode* next;
	struct listnode* prev;
}listnode, *pnode;