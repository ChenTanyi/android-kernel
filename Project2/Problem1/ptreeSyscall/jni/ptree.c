/**
 *Add a new system call for android linux kernel.
 *This system call is called ptree.
 *It returns into the given buffer a depth first search list of process that is running.
 *The return value of the system call is the exact number of running process.
 **/

#include "ptree.h"

static int (*oldcall)(void);

static int __init addsyscall_init(void){//the load of module
	long *syscall = (long*)0xc000d8c4;
	oldcall = (int(*)(void))(syscall[__NR_ptreecall]);
	syscall[__NR_ptreecall] = (unsigned long)sys_ptreecall;
	printk(KERN_INFO "module load!\n");
	return 0;
}

static void __exit addsyscall_exit(void){//the exit of module
	long *syscall = (long*)0xc000d8c4;
	syscall[__NR_ptreecall] = (unsigned long)oldcall;
	printk(KERN_INFO "module exit!\n");
}

static int sys_ptreecall(struct prinfo* buf, int* nr){//the system call
	if (buf == NULL || nr == NULL)
		return -EINVAL;
	struct prinfo *ker_buf;
	int *ker_nr; 
	int total = 1000;
	ker_buf = kcalloc(*nr, sizeof(struct prinfo), GFP_KERNEL);
	if (ker_buf == NULL)
		return -ENOMEM;
	ker_nr = kcalloc(1, sizeof(int), GFP_KERNEL);
	if (ker_nr == NULL){
		kfree(ker_buf);
		return -ENOMEM;
	}
	if (copy_from_user(ker_buf, buf, sizeof(struct prinfo) * 1000) != 0){
		kfree(ker_buf);
		kfree(ker_nr);
		return -EFAULT;
	}
	if (copy_from_user(ker_nr, nr, sizeof(int)) != 0){
		kfree(ker_buf);
		kfree(ker_nr);
		return -EFAULT;
	}
	read_lock(&tasklist_lock);
	*ker_nr = dfs(ker_buf, &total);
	read_unlock(&tasklist_lock);
	if (copy_to_user(buf, ker_buf, sizeof(struct prinfo) * 1000) != 0){
		kfree(ker_buf);
		kfree(ker_nr);
		return -EFAULT;
	}
	if (copy_to_user(nr, ker_nr, sizeof(int)) != 0){
		kfree(ker_buf);
		kfree(ker_nr);
		return -EFAULT;
	}
	kfree(ker_buf);
	kfree(ker_nr);
	return 0;
}

int dfs(struct prinfo *buf, int *nr){//depth first searching for process
	int count = 0;
	int buf_id = 0;
	struct task_struct *cur = &init_task;
	while (cur != NULL){
		if (!is_a_process(cur) || cur->pid == 0){
			cur = get_next_node(cur);
			continue;
		}
		++count;
		if (buf_id < *nr){
			storage_process_to_buf(buf, cur, buf_id);
			++buf_id;
		}
		cur = get_next_node(cur);
	}
	*nr = buf_id;
	return count;
}

void storage_process_to_buf(struct prinfo *buf, struct task_struct *cur, int idx){//save process to buffer
	struct task_struct* first_child, *next_sibling;
	struct prinfo tmp;
	tmp.state = cur->state;
	tmp.pid = cur->pid;
	tmp.parent_pid = cur->parent->pid;
	
	if (has_children(cur)){
		first_child = list_entry(cur->children.next, struct task_struct, sibling);
		tmp.first_child_pid = first_child->pid;
	}
	else tmp.first_child_pid = 0;
	
	if (has_sibling(cur)){
		next_sibling = list_entry(cur->sibling.next, struct task_struct, sibling);
		tmp.next_sibling_pid = next_sibling->pid;
	}
	else tmp.next_sibling_pid = 0;
	
	tmp.uid = task_uid(cur);
	
	tmp.prio = cur->prio;
	tmp.normal_prio = cur->normal_prio;
	tmp.rt_priority = cur->rt_priority;
	
	strncpy(tmp.comm, cur->comm, MAX_COMM);
	
	buf[idx] = tmp;
}

struct task_struct* get_next_node(struct task_struct *cur){//get the next node of process tree, depth first search
	if (has_children(cur))
		return list_entry(cur->children.next, struct task_struct, sibling);
	while (!has_sibling(cur)){
		cur = cur->parent;
		if (cur->pid == 0)
			return NULL;
	}
	return list_entry(cur->sibling.next, struct task_struct, sibling);
}

int has_children(struct task_struct *task){//check if the process has child tasks
	if (task == NULL)
		return 0;
	if (list_empty(&task->children))
		return 0;
	return 1;
}

int has_sibling(struct task_struct *task){//check if the process has sibling tasks
	struct list_head* head = &task->parent->children;
	if (list_is_last(&task->sibling, head))
		return 0;
	return 1;
}

int is_a_process(struct task_struct *task){//check if the task is a process
	if (thread_group_empty(task))//a process with no thread
		return 1;
	else if (thread_group_leader(task))//a process with at least 1 thread
		return 1;
	return 0;
}