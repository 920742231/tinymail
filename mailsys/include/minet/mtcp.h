#ifndef _INET_MTCP_H
#define _INET_MTCP_H
#define BACKLOG     20      /* second parameter in listen*/
#ifndef NULL    
#define NULL    ((void*)0)  /* define NULL as zero void* */
#endif
#include<stddef.h>
#include<minet/msocket.h>  /*_sa,_st and other difinition*/
extern int connect_e(int sockfd,_sa * dest,_st destlen);
extern int accept_e(int sockfd,_sa * source,_st * srclen);
extern void bind_e(int sockfd,_sa * in_st,_st st_len);
extern void s_write_e(int fd,void * buff,size_t bufflen);
extern int s_readline(int fd,char * buff,size_t bufflen);
extern int s_read_e(int fd,void * buff,size_t bufflen);
extern void init_sock(int *so_fd,_sa * __s_addr);
extern void listen_e(int sockfd,int backlog);
#endif