/**
 *This program is to call the ptree syscall.
 *It calls the syscall ptree and print the entire process tree getting from ptree.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "prinfo.h"

void print_tree(struct prinfo*, const int);//print the entire tree
void print_prinfo(int, struct prinfo);//print every tree node 
int is_num(char*);//check if the total string is a number
#define DAUFLT_SIZE 1000;

int main(int argc, char* argv[]){//call for ptree
	int nr;
	if (argc > 1){
		if (!(nr = is_num(argv[1])))
			nr = DAUFLT_SIZE;
	}
	else nr = DAUFLT_SIZE;
	printf("Assign size of %d...\n",nr);
	struct prinfo *buf = calloc(nr, sizeof(struct prinfo));
	if(!buf){
		printf("Could not allocate buffer for buf!\n");
		exit(-1);
	}
	printf("SYSTEM CALL\n");
	int result = syscall(356,buf,&nr);
	if (result != 0) {
		perror("ptree");
		return -1;
	}
	print_tree(buf, nr);
	printf("\nResult = %d\n",nr);
	free(buf);
	return 0;
}

void print_tree(struct prinfo *tree, const int size){//print the entire tree
	int id_stack[size];
	id_stack[0] = 0;
	int num_tabs = 0;
	int i = 0;
	for (i = 0; i < size; i++) {
		while (tree[i].parent_pid != id_stack[num_tabs])
			num_tabs--;
		print_prinfo(num_tabs, tree[i]);
		num_tabs++;
		id_stack[num_tabs] = tree[i].pid;
	}
}

void print_prinfo(int count, struct prinfo p){//print every tree node 
	char returned[count+1];
	int i = 0;
	for (i = 0; i < count; ++i)
		returned[i] = '\t';
	returned[count] = '\0';
	printf("%s%s, %d, %d, %d, %d, %d, %d\n",
		returned, p.comm, p.pid, p.parent_pid, p.prio, p.normal_prio, p.rt_priority, sched_getscheduler(p.pid));
}

int is_num(char* s){//check if the total string is a number
	char* tmp = s;
	if (!s) return 0;
	for (; *tmp ; ++ tmp){
		if (!isdigit(*tmp))
			return 0;
	}
	return atoi(s);
}