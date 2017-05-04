/**
 *This program is to generate a linux executable file.
 *It can change its own scheduler and priority according to the input command line.
 *Than it will calculate the executing time of a number of simple loops according to every input.
 *
 *The command line will use three parameters at most.
 *The second is for policy ('n' or 'f' or 'r')
 *The third is for priority (0 for n and 1~99 for f/r)
 *
 *After running, every you input a number, it will print the executing time of loops in this number 
 **/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

int main(int argc, char *argv[])  
{  

    struct sched_param param;
    param.sched_priority = 99;
	
	//second parameter change the scheduler
    int policy;
    if (argc > 1){
        switch (argv[1][0]){
            case 'f':case 'F': policy = SCHED_FIFO;break;
            case 'r':case 'R': policy = SCHED_RR;break;
            default : 
			 policy = SCHED_NORMAL;
			 param.sched_priority = 0;
        }
    } else {
        policy = SCHED_NORMAL;
        param.sched_priority = 0;
    }

	//third parameter change the priority
    if (argc > 2 && policy != SCHED_NORMAL){
        sscanf(argv[2], "%d", &param.sched_priority);
        if (param.sched_priority > 99) param.sched_priority = 99;
		else if (param.sched_priority < 1) param.sched_priority = 1;
    }

    printf ("Policy %d\n", sched_getscheduler(0));

    if (policy && sched_setscheduler(getpid(), policy, &param) == -1){
        perror("Set Scheduler Error!\n");
        return -1;
    }

    printf ("Policy %d\n", sched_getscheduler(0));

    int i;
	
    int n = 1;
    scanf("%d", &n);
    double y = 0;
	
	//simple loop for test the run time
    while (n > 0){
        long int t = clock();
        for (i = 0 ; i < n ; ++ i) {
            int j = 0;
            for (j = 0 ; j < 1000 ; ++ j)
                y = sqrt(y + 1);
        }
        printf("Executing Time : %f ms\n", 1000.0 * (clock() - t) / CLOCKS_PER_SEC);
        scanf("%d", &n);
    }
    printf("%f\n", y);
    return 0;  
}  