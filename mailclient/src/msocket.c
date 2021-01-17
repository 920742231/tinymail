#include<stdlib.h>
#include<assert.h>
#include<normal.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
struct sockaddr_in * 
init_st(char * host,char * serv,char * trans)
{
    struct servent * serv_info;
    struct hostent * host_info;
    struct sockaddr_in * ser_addr;

    Malloc(struct sockaddr_in,ser_addr,1);

    bzero(ser_addr,sizeof(*ser_addr));

    if((serv_info = getservbyname(serv,trans)))
        ser_addr->sin_port = serv_info->s_port;
    else assert((ser_addr->sin_port = htons(atoi(serv))));

    if((host_info = gethostbyname(host)))
        bcopy(host_info->h_addr,&ser_addr->sin_addr,sizeof(struct in_addr));
    else assert(inet_pton(AF_INET,host,&ser_addr->sin_addr) == 1);

    ser_addr->sin_family = AF_INET;

    return ser_addr;
}
