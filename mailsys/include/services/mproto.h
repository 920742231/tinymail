#ifndef _SERVICES_MPROTO_H
#define _SERVICES_MPROTO_H
#include<msys/mtypes.h>/*definedtyes*/
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