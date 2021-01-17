#include<mio.h>
#include<cmmd.h>
#include<mtypes.h>
#define DEADLOOP 1
#define errexit(msg) ({perror(msg);exit(-1);})
#define Mclose(fd) ({perror("Exit");close(fd);exit(0);})
struct sockaddr_in * init_st(char *,char *,char *);
int main(int argc,char **argv){

    int sockfd,retunval;     /*socket,return value of read*/
    char * serv = "8000";   /*service name or service port*/
    char * trans = "tcp";   /*string of transmission proto*/
    char * host = "jerry";    /*hostname or host ipv4 addr*/
    struct mail_head user_head; /*mail head send to server*/
    struct sockaddr_in * ser_addr;/*server host addrstruct*/

    ser_addr = init_st(host,serv,trans);

    if(strcasecmp(trans,"tcp") == 0)
        assert((sockfd = socket(AF_INET,SOCK_STREAM,0)) > 0);
    else if(!strcasecmp(trans,"udp"))
        assert((sockfd = socket(AF_INET,SOCK_DGRAM,0)) > 0);
    else errexit("Usage:please chose tcp or udp protocol!");
    
    //connect(sockfd,(_sa*)ser_addr,sizeof(*ser_addr));
    assert(!connect(sockfd,(_sa*)ser_addr,sizeof(*ser_addr)));
/*
    while(DEADLOOP){
        /*
        retunval=s_readn(sockfd,&user_head,sizeof(user_head));
        if(retunval == 0)errexit("Server has stopped!");
        else if(retunval != sizeof(user_head))errexit("read");
        else if(services(&user_head) == UNCONN)Mclose(sockfd);
        //
    }
*/

login:
    printf("logind or regist:");
    bzero(&user_head,sizeof(user_head));
    if(!fgets(user_head.usercomd,COMDLEN,stdin))Mclose(sockfd);
    retunval = login_regist(user_head.usercomd)(&user_head,sockfd);

    switch(retunval){
        case 0:Mclose(sockfd);      //server closed
        case 1:goto service;        //login succeed
        default:goto login;         //
    }

service:
    printf("Command#_");
    bzero(&user_head,sizeof(user_head));
    if(!fgets(user_head.usercomd,COMDLEN,stdin))Mclose(sockfd);
    retunval=sys_services(user_head.usercomd)(&user_head,sockfd);

    switch(retunval){
        case 0:Mclose(sockfd);      //server closed
        default:goto service;       //continue service
    }

    close(sockfd);
    return 0;
}