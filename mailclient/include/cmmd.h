#ifndef _CMMD_H
#define _CMMD_H
#define     UNCONN      0           /*sendrecv fail,as connect isclosed.*/

#include<mtypes.h>

#ifndef     MCOMMD      /*corresponding to the enum type of proto-Cmmd_t*/
#define     MCOMMD      
#define     REGIST      "REGIST"    /*REGIST: macro difinition of Regist*/
#define     LOGIND      "LOGIND"    /*LOGIND: macro difinition of Logind*/
#define     MAILTO      "MAILTO"    /*MAILTO: macro difinition of Mailto*/
#define     LISTMS      "LISTMS"    /*LISTMS: macro difinition of Listms*/
#define     RECVMS      "RECVMS"    /*RECVMS: macro difinition of Recvms*/
#define     PASSWD      "PASSWD"    /*PASSWD: macro difinition of Passwd*/
#define     CHNAME      "CHNAME"    /*CHNAME: macro difinition of Chanme*/
#define     ADFRND      "ADFRND"    /*ADFRND: macro difinition of Adfrnd*/
#define     RMFRND      "RMFRND"    /*RMFEND: macro difinition of Rmfrnd*/
#define     SETING      "SETING"    /*SRTING: macro difinition of Seting*/
#define     ACCEPT      "ACCEPT"    /*ACCEPT: macro difinition of Accept*/
#define     LISTFS      "LISTFS"    /*LSITFS: macro difinition of Listfs*/
#define     SERSMS      "SERSMS"    /*SERSMS: macro difinition of Sersms*/
#endif


struct sockaddr_in * init_st(char *,char *,char *);

int (*login_regist(char*))(struct mail_head*,int);
int (*sys_services(char*))(struct mail_head*,int);

#endif