#ifndef _SERVICES_CMMD_H
#define _SERVICES_CMMD_H
#include<services/mproto.h>         /*struct mail_head,MLN-size(mail_hd)*/
#include<muser/account.h>           /*struct u_account,struct about user*/
#include<muser/user.h>

#define     UNCONN      0           /*sendrecv fail,as connect isclosed.*/

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
#define     SERSMS      "SERSMS"    /*SHOWMS: macro difinition of Showms*/

#define     NODELET     0x00000001  /*the last bit in usr_set,1 for keep*/
#define     DELETED     0xfffffffe  /*the last bit in usr_set,0 - nokeep*/

#define     s_nodelet(u_set)        (u_set |= NODELET)/*set no keep mail*/
#define     s_deleted(u_set)        (u_set &= DELETED)/*set keeping mail*/
#define     s_isnodel(u_set)        (u_set << (sizeof(int)-1))/*if no kp*/
#define     s_isdelet(u_set)        (!(s_isnodel(u_set)))/*check if keep*/
#endif

extern int send_message(int sockfd,void * message,size_t);/*sendtosockfd*/
extern int s_regist(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_logind(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_mailto(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_listms(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_recvms(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_passwd(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_chname(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_adfrnd(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_rmfend(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_seting(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_accept(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_logout(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_listfs(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_sersms(struct list_t * ulist_node,struct mail_head * user_md);
extern int s_others(struct list_t * ulist_node,struct mail_head * user_md);

extern int (*s_logctl(struct mail_head*))(struct list_t*,struct mail_head*);
extern int (*s_servic(struct mail_head*))(struct list_t*,struct mail_head*);
#endif