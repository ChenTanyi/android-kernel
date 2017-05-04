#ifndef prinfo_H
#define prinfo_H

#define MAX_COMM 64

struct prinfo{
	pid_t parent_pid;
	pid_t pid;
	pid_t first_child_pid;
	pid_t next_sibling_pid;
	long state;
	long uid;
	char comm[MAX_COMM];
	int prio;
	int normal_prio;
	int rt_priority;
};

#endif