#ifndef _MUSER_USER_H
#define _MUSER_USER_H
#define USERECORDS  ".userecords"/*file of record*/
#define MAILBOXPTH  ".mailboxes"/*mailbox's path.*/
#include<muser/account.h>      /*struct u_account*/
#include<sys/types.h>
#include<sys/stat.h>
#include<pthread.h> /*pthread_mutex_t*/
struct list_t {  /*linkedlist of struct u_account*/
    struct u_account * u_acnt; /*data of the list*/
    Conn_t user_connfd;/*connect socket of client*/
    struct list_t * next;
    struct list_t * prev;
};
extern struct list_t * __user_list;
extern struct list_t * __list_tail;
extern pthread_mutex_t __list_lock;
extern pthread_mutex_t __load_lock;/*lock of writ*/
/*load an user record by the user account number.*/
extern struct u_account * load_user(int u_no);
/*create a directory by username as user mailbox.*/
extern int u_mailbox(struct u_account * user_st);
/*write an new record to file when user register.*/
extern int record_user(struct u_account * user_st);
/*free the linked list of u_acconut by list head.*/
extern void _u_list_free(struct list_t * list_head);
#endif