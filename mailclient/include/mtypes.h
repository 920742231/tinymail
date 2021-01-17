/*some definition and consts in system*/
#ifndef _MTYPES_H
#define _MTYPES_H

#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>

#define f_flags (O_RDWR | O_CREAT | O_EXCL)

#ifndef BOOL
#define BOOL
#define TRUE    1
#define FLASE   0
#endif

#define OPSUCED 0
#define NOSUCHU 1
#define ERRDIRT 2
#define FILEEXT 3
#define NOSUCHF 4
#define OPFAILD 5
#define FULLFND 6
#define NOMATCH 7
#define EREPEAT 8

#define NAMELEN     32
#define PSWDLEN     32
#define FRNDNUM     32
#define COMDLEN     32
#define FNMELEN     64
#define MAILLEN     4096

typedef char Name_t[NAMELEN];
typedef char Pswd_t[PSWDLEN];
typedef char File_t[FNMELEN];
typedef char Comd_t[COMDLEN];
typedef int  Frnd_t[FRNDNUM];
typedef int  Setting;
typedef int  Conn_t;
typedef char Bool_t;
typedef int  Numb_t;
typedef int  Flen_t;

#ifndef NULL
#define NULL    ((void*)0)
#endif

#define _sa   struct sockaddr
#define MLN   sizeof(struct mail_head)
/*enum type of user command,(Cmmd_t)*/
/*
typedef enum {
    Regist,    /*register an account/
    Logind,    /*login to the system/
    Mailto,    /*send mail to friend/
    Listms,    /*list mails of user./
    Recvms,    /*user receive a mail/
    Passwd,    /*user change passwd./
    Chname,    /*user change hisname/
    Adfrnd,    /*user add a friend. /
    Rmfrnd,    /*userremove a frined/
    Seting,    /*user change mailset/
    Accept,    /*accept a friend req/
    Lsitfs     /*list user's friends/
} Comd_t;
*/
/*mail_head:defined the head of mail*/
struct mail_head {
    Comd_t usercomd;/*typeof request*/
    Name_t username;/*user  nickname*/
    Pswd_t password;/*user  password*/
    File_t fliename;/*nameofthe mail*/
    Setting usrsets;/*used on Seting*/
    Numb_t usernumb;/*account number*/
    Flen_t mailsize;/*len ofthe mail*/
};

#endif