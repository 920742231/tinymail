#include<mio.h>
#include<cmmd.h>
#include<time.h>
#include<mtypes.h>
#include<normal.h>
int test(struct mail_head * user_head,int fd){return 0;}
static int s_logind(struct mail_head * user_head,int fd)
{
    int retval;
    time_t nowtime;
    char account[12];
    char recvbuff[strlen("success") + 1];

    printf("Account:");
    if(!fgets(account,12,stdin))Error("Read",-1);
    account[strlen(account) - 1] = 0;
    if((user_head->usernumb = atoi(account)) == 0){
        printf("Usage:the account number is illegal!\n\r");
        return -1;
    }
    printf("Username:");
    if(!fgets(user_head->username,NAMELEN,stdin))
    Error("Read",-1);
    
    printf("Password:");
    tty_mode(0);
    set_noecho();
    if(!fgets(user_head->password,PSWDLEN,stdin))
    Error("Read",-1);
    tty_mode(1);
    
    user_head->username[strlen(user_head->username) - 1] = 0;
    user_head->password[strlen(user_head->password) - 1] = 0;

    if(!s_write(fd,user_head,sizeof(*user_head)))return UNCONN;
    if(!s_readn(fd,recvbuff,strlen("success")))return UNCONN;

    if(strcmp(recvbuff,"success") != 0){
        printf("Failed to login - %s\n\r",recvbuff);
        return -1;
    }

    nowtime = time(NULL);
    printf("Usage:login succeed!\n\r%s",ctime(&nowtime));

    return 1;
}
static int s_regist(struct mail_head * user_head,int fd)
{
    int retval;
    int usrnum;
    char passwd[PSWDLEN];

    printf("Username:");
    if(!fgets(user_head->username,NAMELEN,stdin))
        Error("Read",-1);
    
    printf("Password:");
    tty_mode(0);
    set_noecho();
    if(!fgets(user_head->password,PSWDLEN,stdin))
        Error("Read",-1);
    tty_mode(1);

    printf("\nEnter again:\n");
    tty_mode(0);
    set_noecho();
    if(!fgets(passwd,PSWDLEN,stdin))Error("Read",-1);
    tty_mode(1);

    if(strcasecmp(user_head->password,passwd) != 0){
        printf("Wrong password!\n\r");
        fflush(stdout);
        return 2;
    }

    user_head->username[strlen(user_head->username)-1] = 0;
    user_head->password[strlen(user_head->password)-1] = 0;

    retval = write(fd,user_head,sizeof(*user_head));

    if(retval == UNCONN)return UNCONN;
    else if(retval != sizeof(*user_head))Error("Send",-1);

    retval = s_readn(fd,&usrnum,sizeof(int));

    if(retval == UNCONN)return UNCONN;
    else if(retval != sizeof(int))Error("Recv",-1);
    if(usrnum == -1)
        printf("Usage:failed to regist!\n\r"
               "Maybe the name is existed\n\r");
    else printf("Usage:succeed registing!\n\r"
                "Account number:%d\n\r",usrnum);
    fflush(stdout);

    return 2;
}
static int s_listms(struct mail_head * user_head,int fd)
{
    char content[4096];

    if(!s_write(fd,user_head,sizeof(*user_head)))return UNCONN;

    if(!s_readn(fd,user_head,sizeof(*user_head)))return UNCONN;
    while(user_head->mailsize > 0){
        bzero(content,4096);
        if(s_readn(fd,content,user_head->mailsize) == UNCONN)
            return UNCONN;
        printf("%s",content);
        if(s_readn(fd,user_head,sizeof(*user_head)) == UNCONN)
            return UNCONN;
    }
    if(user_head->mailsize == -OPFAILD)printf("Error\n\r");

    return 2;

}
static int s_listfs(struct mail_head * user_head,int fd)
{
    char content[FRNDNUM*NAMELEN*2];

    bzero(content,FRNDNUM*NAMELEN*2);

    printf("Frineds:\n\r");

    if(!s_write(fd,user_head,sizeof(*user_head)))return UNCONN;
    if(!s_readn(fd,user_head,sizeof(*user_head)))return UNCONN;
    if(!user_head->mailsize || user_head->mailsize == -OPFAILD)
        return 2;   
    
    if(!s_readn(fd,content,user_head->mailsize)) return UNCONN;
   
    printf("%s",content);

    return 2;

}
static int s_adfrnd(struct mail_head * user_head,int fd)
{
    printf("Friend name:");
    if(!fgets(user_head->username,NAMELEN,stdin)){
        perror("Read");
        return 2;
    }
    user_head->username[strlen(user_head->username) - 1] = 0;
    if(!s_write(fd,user_head,sizeof(*user_head)))return UNCONN;
    if(!s_readn(fd,user_head,sizeof(*user_head)))return UNCONN;

    if(user_head->mailsize == OPSUCED)
        printf("Usage:request has been sended.\n");
    else if(user_head->mailsize == -NOSUCHU)
        printf("Usage:failed - no such user!\n");

    return 2;
}
static int s_accept(struct mail_head * user_head,int fd)
{
    char account[12];

    printf("Username:");
    if(!fgets(user_head->username,NAMELEN,stdin)){
        perror("Read");
        return 2;
    }
    
    printf("Account:");
    if(fgets(account,12,stdin) == NULL){
        perror("Read");
        return 2;
    }

    user_head->username[strlen(user_head->username) - 1] = 0;
    account[strlen(account) - 1] = 0;

    if((user_head->usernumb = atoi(account)) == 0){
        printf("Wrong account!\n\r");
        return 2;
    }

    if(!s_write(fd,user_head,sizeof(*user_head)))return UNCONN;
    if(!s_readn(fd,user_head,sizeof(*user_head)))return UNCONN;

    switch(user_head->mailsize){
    case OPSUCED:
        printf("Usage:success add friend %s\n\r",
        user_head->username);
        return 2;
    case -NOSUCHF:
        printf("Usage:no user %s want to make friends with you!\n\r",
        user_head->username);
        return 2;
    case -NOSUCHU:
        printf("Usage:no user %s found in system!\n\r",
        user_head->username);
        return 2;
    case -OPFAILD:
        printf("Usage:operate failed with unknown error!\n\r");
        return 2;
    case -NOMATCH:
        printf("Usage:username and account not matching!\n\r");
        return 2;
    case -EREPEAT:
        printf("Usage:[%s] is already your friend!\n\r",
        user_head->username);
        return 2;
    default:
        break;
    }

    return 2;
}
static int s_mailto(struct mail_head * user_head,int fd)
{
    int mailno;
    char account[12];
    char sendbuff[4096];

    bzero(account,12);
    bzero(sendbuff,4096);

    printf("Filename:");
    if(!fgets(user_head->fliename,FNMELEN,stdin))
        Error("Read",-1);
    printf("Username:");
    if(!fgets(user_head->username,NAMELEN,stdin))
        Error("Read",-1);
    printf("Account:");
    if(!fgets(account,12,stdin))
        Error("Read",-1);
    
    if(!(user_head->usernumb = atoi(account))){
        printf("Usage:illegal account.\n\r");
        return 2;
    }

    user_head->fliename[strlen(user_head->fliename)-1] = 0;
    user_head->username[strlen(user_head->username)-1] = 0;

    if((mailno = open(user_head->fliename,O_RDONLY)) < 0){
        if(errno = ENOENT)
        printf("Usage:this file is not existed.\n\r");
        else if(errno = EISDIR)
        printf("Usage:can not send a dirctory.\n\r");
        else Error("Open",-1);
        return 2;
    }

    if((user_head->mailsize = read(mailno,sendbuff,4096)) < 0){
        Close(mailno);
        if(errno == EISDIR)
        printf("Usage:can not send a dirctory.\n\r");
        else Error("Open",-1);
        return 2;
    }

    if(s_write(fd,user_head,sizeof(*user_head)) != \
        sizeof(*user_head))goto error;
    if(s_readn(fd,user_head,sizeof(*user_head)) != \
        sizeof(*user_head))goto error;
    if(user_head->mailsize < 0)goto over;
    if(s_write(fd,sendbuff,user_head->mailsize) != \
        user_head->mailsize)goto error;
    bzero(sendbuff,4096);
    user_head->mailsize = read(mailno,sendbuff,4096);

    while(user_head->mailsize > 0){
        if(s_write(fd,user_head,sizeof(*user_head)) != \
        sizeof(*user_head))goto error;
        if(s_write(fd,sendbuff,user_head->mailsize) != \
        user_head->mailsize)goto error;
        bzero(sendbuff,4096);
        user_head->mailsize = read(mailno,sendbuff,4096);
    }

    if(user_head->mailsize == 0)
        if(s_write(fd,user_head,sizeof(*user_head)) != \
        sizeof(*user_head))goto error;
    else if(user_head->mailsize < 0){
        perror("Read");
        user_head->mailsize = 0;
        if(s_write(fd,user_head,sizeof(*user_head)) != \
        sizeof(*user_head))goto error;
    }

    if(s_readn(fd,user_head,sizeof(*user_head)) != \
        sizeof(*user_head))goto error;

over:
    switch(user_head->mailsize){
        case -NOSUCHF:
        case -NOSUCHU:
            printf("Usage:no such friend.\n\r");
            break;
        case -FILEEXT:
            printf("Usage:this mail is existed.\n\r");
            break;
        case OPSUCED:
            printf("Usage:send success!\n\r");
            break;
        default:
            printf("Usage:unknown error!\n\r");
            break;
    }
    Close(mailno);
    return 2;

error:
    Close(mailno);
    Close(fd);
    Error("Send", -1);
}
static int s_recvms(struct mail_head * user_head,int fd)
{
    int mailfd;
    char mailbuff[4096];

    printf("Username:");
    if(!fgets(user_head->username,NAMELEN,stdin))
        Error("Read",-1);
    printf("Filename:");
    if(!fgets(user_head->fliename,FNMELEN,stdin))
        Error("Read",-1);
    
    user_head->username[strlen(user_head->username)-1] = 0;
    user_head->fliename[strlen(user_head->fliename)-1] = 0;

    if((mailfd = open(user_head->fliename,f_flags,0644)) < 0){
        perror("Usage");
        return 2;
    }
    if(s_write(fd,user_head,sizeof(*user_head)) == UNCONN)
        goto unconn;
    if(s_readn(fd,user_head,sizeof(*user_head)) == UNCONN)
        goto unconn;

    while(user_head->mailsize > 0)
    {
        bzero(mailbuff,4096);
        if(!s_readn(fd,mailbuff,user_head->mailsize))
            goto unconn;
        if(write(mailfd,mailbuff,user_head->mailsize) \
        != user_head->mailsize){
            perror("Write");
            Close(mailfd);
            return 1;
        }
        //printf("Content:\n\r%s\n\r",mailbuff);
        if(!s_readn(fd,user_head,sizeof(*user_head)))
            goto unconn;
    }
    
    switch(user_head->mailsize){
        case OPSUCED:
            printf("Usage:receive succeed.\n\r");
            break;
        case -NOSUCHF:
            printf("Usage:no such mail.\n\r");
            break;
        case -ERRDIRT:
            printf("Usage:can not receive a directory.\n\r");
            break;
        default:
            printf("Usage:unknown error.\n\r");
    }

    Close(mailfd);
    if(user_head->mailsize)
        if(unlink(user_head->fliename) == -1)
        perror("Unlink");
    return 2;
unconn:
    Close(mailfd);
    return UNCONN;

}
static int s_sersms(struct mail_head * user_head,int fd)
{
    int mailfd;
    char mailbuff[4096];

    printf("Filename:");
    if(!fgets(user_head->fliename,FNMELEN,stdin))
        Error("Read",-1);
    
    user_head->fliename[strlen(user_head->fliename)-1] = 0;

    if((mailfd = open(user_head->fliename,f_flags,0644)) < 0){
        perror("Usage");
        return 2;
    }
    if(s_write(fd,user_head,sizeof(*user_head)) == UNCONN)
        goto unconn;
    if(s_readn(fd,user_head,sizeof(*user_head)) == UNCONN)
        goto unconn;

    while(user_head->mailsize > 0){
        bzero(mailbuff,4096);
        if(!s_readn(fd,mailbuff,user_head->mailsize))
            goto unconn;
        if(write(mailfd,mailbuff,user_head->mailsize) \
        != user_head->mailsize){
            perror("Write");
            Close(mailfd);
            return 1;
        }
        //printf("Content:\n\r%s\n\r",mailbuff);
        if(!s_readn(fd,user_head,sizeof(*user_head)))
            goto unconn;
    }
    
    switch(user_head->mailsize){
        case OPSUCED:
            printf("Usage:receive succeed.\n\r");
            break;
        case -NOSUCHF:
            printf("Usage:no such mail.\n\r");
            break;
        case -ERRDIRT:
            printf("Usage:can not receive a directory.\n\r");
            break;
        default:
            printf("Usage:unknown error.\n\r");
    }

    Close(mailfd);
    if(user_head->mailsize)
        if(unlink(user_head->fliename) == -1)
        perror("Unlink");
    return 2;
unconn:
    Close(mailfd);
    return UNCONN;

}
static void __sys_exit(char*,int);
static int s_exited(struct mail_head * user_head,int fd)
{
    __sys_exit("\n\rSystem exited.\n\r",fd);
}
static int s_helped(struct mail_head * user_head,int fd)
{
    printf("<Command info>\n\r");
    printf("\tlogind\t"
    "--login to system by your account.\n\r");
    printf("\tregist\t"
    "--regist an account for login.\n\r");
    printf("\tadfrnd\t"
    "--request to add a friend.\n\r");
    printf("\taccept\t"
    "--accept add frriend requests.\n\r");
    printf("\tlistfs\t"
    "--show list about your friends.\n\r");
    printf("\tmailto\t"
    "--send mails to your friends.\n\r");
    printf("\tlistms\t"
    "--show mails in your mailbox.\n\r");
    printf("\trecvms\t"
    "--receive mails from users.\n\r");
    printf("\tsersms\t"
    "--receive mails from server.\n\r");
    printf("\texit\t"
    "--exit the system.\n\r");
    printf("\thelp\t"
    "--show command infos.\n\r");
}
static int s_illegal(struct mail_head * user_head,int fd)
{
    printf("Usage:Command [%s] is not supported now!\n\r",
    user_head->usercomd);
    fflush(stdout);
    return 2;
}
#define EXITED "exit"
#define HELPED "help"
int (*login_regist(char * cmmd))(struct mail_head*,int)
{
    cmmd[strlen(cmmd) - 1] = 0;
    if(strcasecmp(cmmd,LOGIND) == 0)return s_logind;
    else if(!strcasecmp(cmmd,REGIST))return s_regist;
    else if(!strcasecmp(cmmd,EXITED))return s_exited;
    else return s_illegal;
}
int (*sys_services(char * cmmd))(struct mail_head*,int)
{
    cmmd[strlen(cmmd) - 1] = 0;
    if(strcasecmp(cmmd,LISTMS) == 0)return s_listms;
    else if(!strcasecmp(cmmd,LISTFS))return s_listfs;
    else if(!strcasecmp(cmmd,ADFRND))return s_adfrnd;
    else if(!strcasecmp(cmmd,ACCEPT))return s_accept;
    else if(!strcasecmp(cmmd,MAILTO))return s_mailto;
    else if(!strcasecmp(cmmd,RECVMS))return s_recvms;
    else if(!strcasecmp(cmmd,SERSMS))return s_sersms;
    else if(!strcasecmp(cmmd,EXITED))return s_exited;
    else if(!strcasecmp(cmmd,HELPED))return s_helped;
    else return s_illegal;
}
static void __sys_exit(char * mstr,int sockfd)
{
    printf("%s",mstr);
    Close(sockfd);
    exit(0);
}