#include <msys/mio.h>
#include <msys/mtypes.h>
#include <msys/normal.h>
#include <msys/threadpool.h>
#include <minet/msocket.h>
#include <minet/mtcp.h>
#include <muser/account.h>
#include <muser/user.h>
#include <services/cmmd.h>
#include <services/mproto.h>
#define DEADLOOP 1
#define ACTUSERS 20
#define DEFTPORT 8000
#define DEFTADDR "localhost"
#define __uacnt  user_node->u_acnt

static int sockfd;
struct list_t * __user_list;
struct list_t * __list_tail;
pthread_mutex_t __load_lock;
pthread_mutex_t __list_lock;
static struct thread_pool *t_pool;
static struct sockaddr_in ser_addr;

void __ser_exit(char *);
void *conn_thread(void *);
void sigint_handler(int signo);

int main()
{
    int * connfd;
    char * ip_str = DEFTADDR;
    unsigned short port = DEFTPORT;
    
    init_in_st(port,ip_str,&ser_addr);
    init_sock(&sockfd,(_sa*)&ser_addr);
    
    signal(SIGINT,sigint_handler);

    if(pthread_mutex_init(&__list_lock,NULL) 
    || pthread_mutex_init(&__load_lock,NULL))
        Error("Init mutex",-1);
        
    t_pool = init_pool(ACTUSERS,ACTUSERS,ACTUSERS);

deadloop:
    connfd = (int*)malloc(sizeof(int));
    if(connfd == NULL){
        perror("malloc");
        goto deadloop;
    }
    *connfd = accept_e(sockfd, NULL, NULL);
    t_pool->add(conn_thread, connfd, t_pool);
    goto deadloop;
}

void *conn_thread(void *arg)
{
    int retval;
    int length;
    struct list_t * user_node;
    struct u_account * u_acnt;
    struct mail_head usr_mhead;

    /*
        if failed to malloc,server will exit,it is not a
        good process,and need a better method.
    */
    Malloc(struct list_t,user_node,1);
    Malloc(struct u_account,u_acnt,1);

    user_node->prev = user_node->next = NULL;
    user_node->user_connfd = *((int*)arg);
    user_node->u_acnt = u_acnt;

login:
    length = s_readn(user_node->user_connfd,&usr_mhead,MLN);
    //printf("%s\n",usr_mhead.usercomd);
    if(length == 0)
    {
        s_logout(user_node,NULL);//connect closed
        return NULL;
    }
    else if(length < 0){
        perror("Read from socket");//read error
        s_logout(user_node,NULL);
        return NULL;
    }
    else 
    retval = s_logctl(&usr_mhead)(user_node,&usr_mhead);
    
    switch (retval)
    {
    case 0:
        if(!send_message(user_node->user_connfd,"success",
        strlen("success"))){
            s_logout(user_node,NULL);//close the socked,
            return NULL;                  //free the node
        }
        goto service;
    case 1:
        if(!send_message(user_node->user_connfd,"failure",
        strlen("failure"))){
            s_logout(user_node,NULL);
            return NULL;
        }
        break;
    default:
        if(!send_message(user_node->user_connfd, &retval,
        sizeof(int))){
            s_logout(user_node,NULL);
            return NULL;
        }
    }

    goto login;

service:
    length = s_readn(user_node->user_connfd,&usr_mhead,MLN);
    if(length == 0){
        s_logout(user_node,NULL);//connect closed
        return NULL;
    }
    else if(length < 0){
        perror("Read from socket");//read error
        s_logout(user_node,NULL);
        return NULL;
    }
    else if(!s_servic(&usr_mhead)(user_node,&usr_mhead)){
        s_logout(user_node,NULL);
        return NULL;
    }
    
    goto service;

}

void sigint_handler(int signo)
{
    Close(sockfd);
    t_pool->destory(t_pool);
    if(pthread_mutex_destroy(&__list_lock)
    || pthread_mutex_destroy(&__load_lock))
        perror("Destroy mutex");
    __ser_exit("Sys stop!");
}
void __ser_exit(char * msg)
{
    printf("\n\r%s\n\r",msg);
    exit(0);
}