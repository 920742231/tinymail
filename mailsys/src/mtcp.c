#include<minet/mtcp.h>
#include<minet/msocket.h>
#include<msys/normal.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<unistd.h>
int accept_e(int sockfd,_sa * source,_st * srclen)
{
    int connfd = accept(sockfd,source,srclen);
    if(connfd <= 0) perror("accept"); 
    else return (connfd);
}

void bind_e(int sockfd,_sa * in_st,_st st_len)
{
    assert((bind(sockfd,in_st,st_len) == 0));
}
void s_write_e(int sockfd,void * content,size_t contlen)
{
    assert((write(sockfd,content,contlen) == contlen));
}

int s_readline(int fd,char * buff,size_t bufflen)
{
    int readn;
    int offset;

    offset = 0;
    while(((readn = read(fd,&(buff[offset]),1)) == 1) 
    && (offset < bufflen) && (buff[offset] != '\n'))
        offset++;

    if(readn < 0)Error("readline",-1);
    
    return readn;
}

int s_read_e(int fd,void * buff,size_t bufflen)
{
    assert((read(fd,buff,bufflen) > 0));
}

void listen_e(int sockfd,int backlog)
{
    assert(listen(sockfd,BACKLOG) == 0);
}

int connect_e(int sockfd,_sa * dest,_st destlen)
{
    assert(connect(sockfd,dest,destlen));
}

void init_sock(int *so_fd,_sa * __s_addr)
{
    *so_fd = s_socket_e(AF_INET,SOCK_STREAM,0);
    bind_e(*so_fd,__s_addr,sizeof(*__s_addr));
    listen_e(*so_fd,BACKLOG);
}