#ifndef _INET_MSOCKET_H
#define _INET_MSOCKET_H
#include<netinet/in.h>       /*some structures used , like struct sockaddr_in*/
#include<sys/socket.h>       /*struct sockaddr,socklen_t*/
#define _sa struct sockaddr  /*simplified name for sockaddr,convenient to use*/
#define _st socklen_t        /*simplified name of socklen_t,convenient to use*/
extern void print_addr(const struct sockaddr_in * addr);/* print ip and port */
extern int s_socket_e(int domain,int type,int protocol);/* create a socket   */
extern void init_in_st(const int port,char * ipstr,struct sockaddr_in * in_st);
extern int setsockopt_e(int fd,int level,int onme,const void *optval,_st olen);
extern void s_close_e(int sockfd);
#endif