#ifndef _THREADPOOL_H
#define _THREADPOOL_H
#define TRUE 	1
#define FALSE	0
#include<pthread.h>
#include<unistd.h>
#include<stddef.h>
#define EMUTEXINIT	1
#define ECONDINIT	2
#define EPTCREATE	3
#define EMUTEXLOCK	4
#define ECONDWAIT	5
#define EMUTEXULCK	6
#define ECONDSIGN	7
#define ETHRDJOIN	8
#define ECONDCAST	9
#define ECONDOSTY	10
#define EMUTEXDES	11
#define EMALLOC		1
typedef int num_t;
typedef char status;

#ifndef Malloc
#define Malloc(type,value,size) ({       \
value = (type*)malloc(sizeof(type)*size);\
assert(value);})
#endif

struct task_node {
	void * (*t_task)(void*);
	void * t_arg;
};
struct list_node {
	void * ldata;
	struct list_node *next,*prev;
};
struct thread_pool {
	num_t threads_max;
	num_t threads_min;
	num_t tasks_max;
	num_t cur_task_nums;
	num_t cur_threads_nums;	
	//num_t exit_thread_num;
	pthread_t * threads;
	//pthread_t daemon_tid;

	pthread_mutex_t lock_on_p;
	pthread_cond_t t_null_cond;
	pthread_cond_t t_full_cond;

	struct list_node * list_head;
	struct list_node * list_tail;

#define __head_t	list_head->ldata
#define __t_next	list_head->next
//#define __T_BATCH	threads_min / 2
//#define __ADD_LIMIT	tasks_max / 2

	int (*add)(void *(*)(void*),void *,struct thread_pool*);
	void (*destory)(struct thread_pool*);

	status isclosed;
};
struct thread_pool * 
init_pool(num_t threads_max,num_t threads_min,num_t tasks_max);
#endif
