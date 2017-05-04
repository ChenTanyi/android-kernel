#ifndef ptree_H
#define ptree_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/list.h>
#include <asm-generic/errno-base.h>
#include <linux/stddef.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include "prinfo.h"
MODULE_LICENSE("Dual BSD/GPL");
#define __NR_ptreecall 356

static int __init addsyscall_init(void);//the load of module
static void __exit addsyscall_exit(void);//the exit of module
static int sys_ptreecall(struct prinfo*, int*);//the system call

module_init(addsyscall_init);
module_exit(addsyscall_exit);


int dfs(struct prinfo *buf, int *nr);//depth first searching for process
void storage_process_to_buf(struct prinfo *buf, struct task_struct *cur, int idx);//save process to buffer
struct task_struct* get_next_node(struct task_struct *cur);//get the next node of process tree, depth first search
int has_children(struct task_struct *task);//check if the process has child tasks
int has_sibling(struct task_struct *task);//check if the process has sibling tasks
int is_a_process(struct task_struct *task);//check if the task is a process

#endif