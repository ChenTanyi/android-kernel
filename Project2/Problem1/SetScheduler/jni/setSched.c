/**
 *This program is to set the scheduler for the android applications.
 *It can change the exact application: processtest.apk.
 *Or it can change all the descendants of the process zygote.
 *
 *The command line will use four parameters at most.
 *The second is policy ('n' or 'f' or 'r')
 *The third is priority (0 for n and 1~99 for f/r)
 *The fourth is mode ('a' for change all, others or none for change processtest.apk)
 **/

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <ctype.h>

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

int main(int argc, char *argv[])  
{  
    struct sched_param param;  
	
	int nr = 1000;
	struct prinfo* buf = calloc(1000, sizeof(struct prinfo));
	if (!buf){
		perror("Could not allocate buffer!\n");
		exit(-1);
	}
	
	//get max priority of the two scheduler
	int maxFIFO = sched_get_priority_max(SCHED_FIFO);
	int maxRR = sched_get_priority_max(SCHED_RR);
	
	//syscall to get the process tree
	if (syscall(356, buf, &nr) != 0){
		perror("ptree\n");
		exit(-1);
	}
	
	int policy = SCHED_NORMAL;
	param.sched_priority = 0;
	
	//second parameter change the scheduler
	if (argc > 1){
		switch (argv[1][0]){
			case 'f': case 'F': policy = SCHED_FIFO;break;
			case 'r': case 'R': policy = SCHED_RR;break;
		}
	}
	
	//third parameter change priority
	if (argc > 2){
		sscanf(argv[2], "%d", &param.sched_priority);
		switch (policy){
			case SCHED_FIFO:
				if (param.sched_priority > maxFIFO)
					param.sched_priority = maxFIFO;
				else if (param.sched_priority < 0)
					param.sched_priority = 0;
				break;
			case SCHED_RR:
				if (param.sched_priority > maxRR)
					param.sched_priority = maxRR;
				else if (param.sched_priority < 0)
					param.sched_priority = 0;
				break;
			default: param.sched_priority = 0;
		}
	}
	
	//fourth parameter change all descendants or the exact process
	int i;
	if (argc > 3 && (argv[3][0] == 'a' || argv[3][0] == 'A')){
		pid_t maxpid = 0;
		for (i = 0 ; i < nr ; ++ i){
			if (maxpid < buf[i].pid) maxpid = buf[i].pid;
		}
		int* isChild = calloc(maxpid + 1, sizeof(int));
		memset(isChild, 0, sizeof(isChild));
		for (i = 0 ; i < nr ; ++ i){
			if (strcmp(buf[i].comm, "main") == 0)
				isChild[buf[i].pid] = 1;
			if (isChild[buf[i].parent_pid]){
				isChild[buf[i].pid] = 1;
				if (sched_setscheduler(buf[i].pid, policy, &param) == -1){
					perror("sched_setscheduler() error!\n");
					free(isChild);
					free(buf);
					exit(-1);
				}
			}
		}
		free(isChild);
	} else {
		for (i = 0 ; i < nr ; ++ i){
			if (strcmp(buf[i].comm, "est.processtest") == 0){
				if (sched_setscheduler(buf[i].pid, policy, &param) == -1){
					perror("sched_setscheduler() error!\n");
					free(buf);
					exit(-1);
				}
			}
		}
	}
	
	free(buf);
    return 0;  
}  