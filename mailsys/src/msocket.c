#include<sys/socket.h>
#include<minet/msocket.h>
#include<assert.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<stdio.h>
extern void print_addr(const struct sockaddr_in * addr)
{

}
extern int s_socket_e(int domain,int type,int protocol)
{
    int sockfd;
    assert((sockfd = socket(domain,type,protocol)) > 0);
    return sockfd;
}
extern void 
init_in_st(const int port,char * ipstr,struct sockaddr_in * in_st)
{
    in_st->sin_port = htons(port);
    if(inet_pton(AF_INET,ipstr,&(in_st->sin_addr)) < 0)
        in_st->sin_addr.s_addr = htonl(INADDR_ANY);
    in_st->sin_family = AF_INET;
}
extern int 
setsockopt_e(int fd,int level,int optname,const void *optval,_st optlen){}

extern void s_close_e(int sockfd)
{
    if(close(sockfd))perror("close");
}