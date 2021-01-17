	#include <stdlib.h>					    //malloc,exit
	#include <stdio.h>					    //perror
	#include <assert.h>					    //assert
	#include <errno.h>					    //error number
	#include <stddef.h>					    //
	#include <sys/signal.h>				    //pthread_join
	#include <msys/threadpool.h>    		//
	static void __destory_pool();
	static void __err_exit(char *, int);
	static void __free_list(struct list_node *);
	static void * __daemon_thread(void *);
	static void * __do_thread(void *);
	static int __add_task(void *(*)(void *), void *, struct thread_pool *);
struct thread_pool *
init_pool(num_t threads_max, num_t threads_min, num_t tasks_max)
{
	struct thread_pool *this_pool;
	pthread_t *threads;

	Malloc(struct thread_pool, this_pool, 1);
	Malloc(pthread_t, threads, threads_max);

	if (pthread_mutex_init(&(this_pool->lock_on_p), NULL) != 0)
		__err_exit("mutex init", -EMUTEXINIT);
	if (pthread_cond_init(&(this_pool->t_null_cond), NULL) != 0)
		__err_exit("ncond init", -ECONDINIT);
	if (pthread_cond_init(&(this_pool->t_full_cond), NULL) != 0)
		__err_exit("fcond init", -ECONDINIT);

	this_pool->threads = threads;
	this_pool->threads_max = threads_max;
	this_pool->threads_min = threads_min;
	this_pool->tasks_max = tasks_max;
	this_pool->cur_threads_nums = threads_min;
	this_pool->cur_task_nums = 0;
	this_pool->list_head = this_pool->list_tail = NULL;
	this_pool->isclosed = FALSE;
	this_pool->add = __add_task;
	this_pool->destory = __destory_pool;

	for (int i = 0; i < threads_min; i++)
		if (pthread_create(&threads[i], NULL, __do_thread, (void *)this_pool))
			__err_exit("create thread", -EPTCREATE);
/*
	if(pthread_create(&(this_pool->daemon_tid),NULL,__daemon_thread,this_pool))
		__err_exit("create thread",-EPTCREATE);
*/
	return this_pool;
}
static void *__do_thread(void *arg)
{
	struct task_node *t_node;
	struct thread_pool *this_pool = (struct thread_pool *)arg;
deadloop:
	if (pthread_mutex_lock(&(this_pool->lock_on_p)))
		__err_exit("mutex lock", -EMUTEXLOCK);
	/*
	if(this_pool->exit_thread_num)
	{
		this_pool->exit_thread_num--;
		if(pthread_mutex_unlock(&(this_pool->lock_on_p)))
			__err_exit("mutex unlock",-EMUTEXULCK);
		pthread_exit(NULL);
	}
	*/
	while (!(this_pool->list_head) && (!this_pool->isclosed))
		if (pthread_cond_wait(&(this_pool->t_null_cond), &(this_pool->lock_on_p)))
			__err_exit("cond wait", -ECONDWAIT);

	if (this_pool->isclosed)
	{
		if (pthread_mutex_unlock(&(this_pool->lock_on_p)))
			__err_exit("mutex unlock", -EMUTEXULCK);
		pthread_exit(NULL);
	}

	t_node = (struct task_node *)this_pool->__head_t;
	if (this_pool->list_head == this_pool->list_tail)
		this_pool->list_head = this_pool->list_tail = NULL;
	else
		this_pool->list_head = this_pool->__t_next;
	this_pool->cur_task_nums--;
	if (pthread_cond_signal(&(this_pool->t_full_cond)))
		__err_exit("fcond signal", -ECONDSIGN);

	if (pthread_mutex_unlock(&(this_pool->lock_on_p)))
		__err_exit("mutex unlock", -EMUTEXULCK);

	t_node->t_task(t_node->t_arg);

	goto deadloop;
};
static int
__add_task(void *(*t_task)(void *), void *arg, struct thread_pool *this_pool)
{
	struct task_node *t_node;
	struct list_node *l_node;
	t_node = (struct task_node *)malloc(sizeof(struct task_node));
	if (!t_node)return -EMALLOC;
	l_node = (struct list_node *)malloc(sizeof(struct list_node));
	if (!l_node)return -EMALLOC;
	t_node->t_task = t_task;
	t_node->t_arg = arg;
	l_node->ldata = (void *)t_node;
	l_node->next = l_node->next = NULL;

	if (pthread_mutex_lock(&(this_pool->lock_on_p)))
		return -EMUTEXLOCK;

	while ((this_pool->cur_task_nums == this_pool->tasks_max) &&
		   (!this_pool->isclosed))
		if (pthread_cond_wait(&(this_pool->t_full_cond), &(this_pool->lock_on_p)))
			__err_exit("fcond wait", -ECONDWAIT);

	if (this_pool->isclosed)
	{
		if (pthread_mutex_unlock(&(this_pool->lock_on_p)))
			__err_exit("mutex unlock", -EMUTEXULCK);
		else
			return 0;
	}

	if (!(this_pool->list_head))
	{
		this_pool->list_tail = this_pool->list_head = l_node;
		if (pthread_cond_signal(&(this_pool->t_null_cond)))
			__err_exit("ncond signal", -ECONDSIGN);
	}
	else
	{
		this_pool->list_tail->next = l_node;
		l_node->prev = this_pool->list_tail;
		this_pool->list_tail = l_node;
	}

	this_pool->cur_task_nums++;

	if (pthread_mutex_unlock(&(this_pool->lock_on_p)))
		__err_exit("mutex unlock", -EMUTEXULCK);

	return 0;
}
static void __destory_pool(struct thread_pool *this_pool)
{

	this_pool->isclosed = TRUE;

	if (pthread_cond_broadcast(&(this_pool->t_full_cond)))
		__err_exit("cond broadcast", -ECONDCAST);
	if (pthread_cond_broadcast(&(this_pool->t_null_cond)))
		__err_exit("cond broadcast", -ECONDCAST);

	for (int i = 0; i < this_pool->threads_min; i++)
	{
		if (pthread_join(this_pool->threads[i], NULL))
			__err_exit("jion", -ETHRDJOIN);
		//printf("exit\n\r");
	}
	/*
	if(pthread_join(this_pool->daemon_tid,NULL))
		__err_exit("join",-ETHRDJOIN);
	*/
	free(this_pool->threads);
	__free_list(this_pool->list_head);

	if (pthread_cond_destroy(&(this_pool->t_null_cond)))
		__err_exit("cond destory", -ECONDOSTY);
	if (pthread_cond_destroy(&(this_pool->t_full_cond)))
		__err_exit("cond destory", -ECONDOSTY);
	if (pthread_mutex_destroy(&(this_pool->lock_on_p)))
		__err_exit("mutex destory", -EMUTEXDES);

	free(this_pool);
}
static inline void __err_exit(char *msg, int no)
{
	perror(msg);
	exit(no);
}
static void __free_list(struct list_node *list_head)
{
	struct list_node *node;
	while (list_head)
	{
		node = list_head;
		list_head = list_head->next;
		free(node->ldata);
		free(node);
	}
}
