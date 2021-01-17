    #include<msys/normal.h> /* some normal functions,like Malloc,Error. */
    #include<msys/mtypes.h> /*some definesd types about the mail system */
    #include<services/cmmd.h>  /*head file of cmmd.c,some func and macro*/
    #include<services/mproto.h> /*head file about mail proto (mail_head)*/
    #include<muser/user.h> /*load user and record user,login and regist */
    #include<muser/account.h>/*head file about account(struct u_account)*/
    #include<string.h>/*head file about string,(strcpy,strcmp,sprintf..)*/
    #include<msys/mio.h>          /*(s_fread,s_readn,s_fwrie,s_write...)*/
    #include<time.h>              /*time(),ctime,get current system time*/
    #define user_st ulist_node->u_acnt
static int __record_user(struct u_account * u_acnt,Numb_t usrnumb);

extern int send_message(int sockfd,void * message,size_t msglen)
{
    return s_write(sockfd,message,msglen);
}
extern void __add_node(struct list_t **list_tail,struct list_t * new_node);
static void __rmv_node(struct list_t **list_head,struct list_t * rmd_node);
static int __check_frnd(struct list_t * u_node,struct mail_head * user_md);
extern int s_regist(struct list_t * ulist_node,struct mail_head * user_md)
{
    bzero(user_st,sizeof(*user_st));/*set struct zero as initial user's */
    /* settings is null,like usr_set is 0,and friends is null...        */
    strcpy(user_st->username,user_md->username);/*set username byrequest*/
    strcpy(user_st->password,user_md->password);/*set password byrequest*/
    //user_st->usr_set = user_md->usrsets;/*initial user settings is 0. */
    for(int i=0;i < FRNDNUM;i++)user_st->userfrnd[i] = -1;
    user_st->isexistd = TRUE;/*new user state is exist in mail system.  */
    user_st->usernumb = record_user(user_st);/*write user record to file*/
    return user_st->usernumb;/*return user number to show regits result.*/
}
extern int s_logind(struct list_t * ulist_node,struct mail_head * user_md)
{
    struct u_account * u_acnt;/*save the user record loaded */
    int cordnum = _uno_to_cno(user_md->usernumb);
    if((u_acnt = load_user(cordnum)) == NULL)return 1;
    /*check the username and password,return 1 as user login successfuly*/
    if(strcmp(u_acnt->username,user_md->username) != 0 || 
        strcmp(u_acnt->password,user_md->password) != 0)
        return 1;  /* password username are not right,return -1 as fail.*/
    bcopy(u_acnt,user_st,ACNTSIZE);  /*copy struct u_account to user_st.*/
    ulist_node->prev = ulist_node->next = NULL;
    __add_node(&__list_tail,ulist_node);/*add new node to the user list.*/
    free(u_acnt);/*free the template pointer u_acnt*/
    return 0;/*login successfully*/
}
extern int s_mailto(struct list_t * ulist_node,struct mail_head * user_md)
{
    int fd;
    char buff[4096];
    char mailname[256];

    /*
        need a control of user receiver(for that user can only send mail to
        his friends)
    */
    if(__check_frnd(ulist_node,user_md)){    /*refused to send this mail.*/
        user_md->mailsize = -NOSUCHF; /*as receiver is not user's friend.*/
        return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
    }
    /*mail name is consisted of .mail/[receiver]/[sendername]/[filename].*/
    sprintf(mailname,"%s/%s/%s/%s",
    MAILBOXPTH,user_md->username,user_st->username,user_md->fliename);

    if((fd = open(mailname,O_WRONLY | O_CREAT | O_EXCL,0644)) < 0){
        if(errno == EEXIST)user_md->mailsize = -FILEEXT;
        else if(errno == ENOENT)user_md->mailsize = -NOSUCHU;
        return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
    }

    if(s_write(ulist_node->user_connfd,user_md,sizeof(*user_md)) \
    == UNCONN)goto unconn;

    while((user_md->mailsize > 0) && !strcasecmp(user_md->usercomd,MAILTO))
    { 
        bzero(buff,4096);       /*the signal of mail end is mailsize = -1*/
        if(s_readn(ulist_node->user_connfd,buff,user_md->mailsize) \
            == UNCONN)goto unconn;     /*read content of mail from socket*/
        s_write(fd,buff,user_md->mailsize);  /*write user mail into file.*/
        if(s_readn(ulist_node->user_connfd,user_md,sizeof(*user_md)) \
            == UNCONN)goto unconn;    
    }
    Close(fd);
    user_md->mailsize = OPSUCED;    /*signal of the successful operation.*/
    return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
unconn:
    Close(fd);
    return UNCONN;
}
/*
extern int s_listms(struct list_t * ulist_node,struct mail_head * user_md)
{
    FILE * fp;  /*file pointer of popen ls -R to show the user's mailbox.//
    char content[4096]; /*buffer that contains the content read from file//
    char command[128]; /*linux command "ls -R [..]",show files in mailbox//
    struct mail_head serv_md; /*head of server mail that sended to client//
    bzero(command,64);              //  clear the buffer
    bzero(&serv_md,sizeof(serv_md));// 
    serv_md.mailsize = 4096,strcasecmp(serv_md.usercomd,LISTMS);/*sethead//
    /*find user's mailbox name by the conten of user's mail head-username//
    sprintf(command,"ls -R %s/%s",MAILBOXPTH,user_st->username);
    /*use popen to read the result of ls -R [...] send the content to usr//
    if((fp = popen(command,"r")) == NULL){
        perror("popen-(ls -R [])");       /*message about error of perror//
        return 1;
    }
    while(!feof(fp)){                   /*end of file,stop read and write//
        bzero(content,4096);        //clear the buffer
        s_fread(content,4096,1,fp);/*read contents about mails from popen//
        if((s_write(ulist_node->user_connfd,&serv_md,sizeof(serv_md)) \
            == UNCONN) || (s_write(ulist_node->user_connfd,content,4096) \
            == UNCONN)){
            pclose(fp);         /*close the process from popen.//
            return UNCONN;      /*write return 0 as the connect is closed//
        }
    }
    pclose(fp);/*close process from popen and return 1 as operate success//
    return 1;
}
*/
extern int s_listms(struct list_t * ulist_node,struct mail_head * user_md)
{
    FILE * fp;
    char command[128];
    char content[4096];

    bzero(command,128);
    sprintf(command,"ls -aR %s/%s",MAILBOXPTH,user_st->username);

    user_md->mailsize = -OPFAILD;
    if((fp = popen(command,"r")) == NULL){
        perror("popen (ls -R)");
        return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
    }

    bzero(content,4096);
    user_md->mailsize = read(fileno(fp),content,4096);
    while(user_md->mailsize > 0){
        if(s_write(ulist_node->user_connfd,user_md,sizeof(*user_md)) \
        == UNCONN){
            Pclose(fp);
            return UNCONN;
        }
        if(s_write(ulist_node->user_connfd,content,user_md->mailsize) \
        == UNCONN){
            Pclose(fp);
            return UNCONN;
        }
        bzero(content,4096);
        user_md->mailsize = read(fileno(fp),content,4096);
    }

    Pclose(fp);

    user_md->mailsize = user_md->mailsize == 0 ? OPSUCED : -OPFAILD;
    return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));

}
extern int s_recvms(struct list_t * ulist_node,struct mail_head * user_md)
{
    int mailfd;
    int readlen;
    int userset;
    char mailname[256];
    char sendbuff[4096];

    bzero(mailname,256);
    sprintf(mailname,"%s/%s/%s/%s", /*consisted of .mail/user/sender/file*/
    MAILBOXPTH,user_st->username,user_md->username,user_md->fliename);
    user_md->mailsize = OPSUCED;
    if((mailfd = open(mailname,O_RDONLY)) < 0){    /*error open the mail.*/
        perror("receive mail");        /*print details of the open error.*/
        if(errno == ENOENT)user_md->mailsize = -NOSUCHF;
        return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
    }

    bzero(sendbuff,4096);
    while((readlen = read(mailfd,sendbuff,4096)) > 0){  /*read from mail.*/
    //  printf("Buff:%s\n\r",sendbuff);
        user_md->mailsize = readlen;/*tell client how many bytes to recv.*/
        if(s_write(ulist_node->user_connfd,user_md,sizeof(*user_md)) 
            == UNCONN)goto unconn;    /*client has closed the connection.*/
        if(s_write(ulist_node->user_connfd,sendbuff,readlen) == UNCONN)
            goto unconn;
        bzero(sendbuff,4096);
    }

    /*read over,maybe error or end of file,set flag to tell user file end*/
    user_md->mailsize = (errno == EISDIR) ? -ERRDIRT : OPSUCED;
    Close(mailfd);

    userset = ulist_node->u_acnt->usr_set;
    if((user_md->mailsize == OPSUCED)&&s_isdelet(userset))unlink(mailname);

    return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
unconn:
    Close(mailfd);
    return UNCONN;
}
extern int s_passwd(struct list_t * ulist_node,struct mail_head * user_md)
{
    struct u_account newacnt;

    bcopy(user_st,&newacnt,ACNTSIZE);
    strcpy(newacnt.password,user_md->password);
    if(__record_user(&newacnt,user_st->usernumb) == -1)
        strcpy(user_md->password,user_st->password);
    else strcpy(user_st->password,user_md->password);

    return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
}
extern int s_chname(struct list_t * ulist_node,struct mail_head * user_md)
{
    struct u_account newacnt;

    bcopy(user_st,&newacnt,ACNTSIZE);
    strcpy(newacnt.username,user_md->username);
    if(__record_user(&newacnt,user_st->usernumb) == -1)
        strcpy(user_md->username,user_st->username);
    else strcpy(user_st->username,user_md->username);

    return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
}
extern int s_adfrnd(struct list_t * ulist_node,struct mail_head * user_md)
{
    int mailfd;
    int length;
    time_t nowtime;
    char mailname[256];
    char mailbuff[1024];

    bzero(mailname,256);
    sprintf(mailname,"%s/%s/[%s-make-friends]",
    MAILBOXPTH,user_md->username,user_st->username);

    user_md->mailsize = -OPFAILD;

    if((mailfd = open(mailname,O_RDWR | O_CREAT,0644)) < 0){
        if(errno == ENOENT)user_md->mailsize = -NOSUCHU;
        return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
    }
    
    length = 10 + strlen(\
    "[Make friends request]\n"
    " From:\n"
    " Account:\n"
    " Time:"
    " Using \'accept\' command to accept the request.\n") \
    + strlen(ctime(&nowtime)) \
    + strlen(user_st->username);

    nowtime = time(NULL);
    bzero(mailbuff,256);
    sprintf(mailbuff,
    "[Make friends request]\n"
    " From:%s\n"
    " Account:%10d\n"
    " Time:%s"
    " Using \'accept\' command to accept the request.\n",
    user_st->username,user_st->usernumb,ctime(&nowtime));

    if(s_write(mailfd,mailbuff,length) == length)user_md->mailsize = OPSUCED;

    Close(mailfd);

    return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
}
extern int s_rmfend(struct list_t * ulist_node,struct mail_head * user_md)
{
    struct u_account * fndacnt;

    user_md->mailsize = -OPFAILD;

    for(int i=0;i < FRNDNUM;i++){
        if(user_st->userfrnd[i] != user_md->usernumb)continue;
        user_st->userfrnd[i] = -1;
    }

    if(__record_user(user_st,user_st->usernumb) == 0)
        user_md->mailsize = OPSUCED;

    if((fndacnt = load_user(user_md->usernumb)) == NULL)
        return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));

    for(int i=0;i < FRNDNUM;i++){
        if(fndacnt->userfrnd[i] != user_st->usernumb)continue;
        fndacnt->userfrnd[i] = -1;
        break;
    }

    __record_user(fndacnt,fndacnt->usernumb);

    free(fndacnt);

    return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));

}
extern int s_seting(struct list_t * ulist_node,struct mail_head * user_md)
{
    struct u_account newacnt;

    bcopy(user_st,&newacnt,ACNTSIZE);
    newacnt.usr_set = user_md->usrsets;
    if(__record_user(&newacnt,user_st->usernumb) == -1)
        user_md->usrsets = user_st->usr_set;
    else user_st->usr_set = user_md->usrsets;

    return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
}
extern int s_accept(struct list_t * ulist_node,struct mail_head * user_md)
{
    int i;
    int mailfd;
    int length;
    int cordno;
    time_t nowtime;
    char mailname[256];
    char mailbuff[256];
    struct u_account * fndacnt;

    bzero(mailname,256);
    sprintf(mailname,"%s/%s/[%s-make-friends]",
    MAILBOXPTH,user_st->username,user_md->username);

    //printf("%s|\n\r",mailname);
    
    user_md->mailsize = -OPFAILD;
    if(access(mailname,F_OK) != 0){         /*check if request is existed*/
        if(errno = ENOENT)user_md->mailsize = -NOSUCHF;
        return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
    }

    cordno = user_md->usernumb;
    //printf("%d\n\r",cordno);
    if((fndacnt = load_user(_uno_to_cno(cordno))) == NULL){
        user_md->mailsize = -NOSUCHU;   /*failed to load request's sender*/
        return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
    }

    if(strcmp(fndacnt->username,user_md->username)){
        free(fndacnt);
        user_md->mailsize = -NOMATCH;
        return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
    }

    if((user_st->frndnums == FRNDNUM) || (fndacnt->frndnums == FRNDNUM))
    {                 /*user has already get 32 friends,can not make more*/
        free(fndacnt);
        user_md->mailsize = -FULLFND;
        return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
    }

    /*
        create directory of friend's mails
    */
    bzero(mailname, 256);
    sprintf(mailname, "%s/%s/%s",
    MAILBOXPTH, user_st->username, user_md->username);
    if (mkdir(mailname, 0700) == -1){
    free(fndacnt);
    if(errno == EEXIST)user_md->mailsize = -EREPEAT;
    return s_write(ulist_node->user_connfd, user_md, sizeof(*user_md));
    }

    bzero(mailname, 256);
    sprintf(mailname, "%s/%s/%s",
    MAILBOXPTH, user_md->username, user_st->username);
    if (mkdir(mailname, 0700) == -1){
    free(fndacnt);
    if(errno == EEXIST)user_md->mailsize = -EREPEAT;
    return s_write(ulist_node->user_connfd, user_md, sizeof(*user_md));
    }

    for(i = 0;i < FRNDNUM;i++){
        if(user_st->userfrnd[i] != -1)continue;
        user_st->userfrnd[i] = user_md->usernumb;
        break;
    }

    for(i = 0;i < FRNDNUM;i++){
        //printf("frnd:%d\n\r",fndacnt->userfrnd[i]);
        if(fndacnt->userfrnd[i] != -1)continue;
        fndacnt->userfrnd[i] = user_st->usernumb;
        break;
    }

    if((__record_user(user_st,user_st->usernumb) != 0)
    || (__record_user(fndacnt,fndacnt->usernumb) != 0)){
        free(fndacnt);
        return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
    }
    
    user_md->mailsize = OPSUCED;

    //printf("frnd:%d\n\r",fndacnt->userfrnd[i]);
    free(fndacnt);

    bzero(mailname,256);
    sprintf(mailname,"%s/%s/[%s-accepted]",
    MAILBOXPTH,user_md->username,user_st->username);
    if((mailfd = open(mailname,O_RDWR | O_CREAT,0644)) < 0){
        perror("Open on create");
        return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
    }

    length = 10 + strlen(\
    "[Make friends response]\n"
    " From:\n"
    " Account:\n"
    " Time:"
    " [] has accepted your request!\n") \
    + strlen(ctime(&nowtime)) \
    + strlen(user_st->username) * 2;
    nowtime = time(NULL);
    sprintf(mailbuff,
    "[Make friends response]\n"
    " From:%s\n"
    " Account:%10d\n"
    " Time:%s"
    " [%s] has accepted your request!\n",
    user_st->username,user_st->usernumb,ctime(&nowtime),user_st->username);

    s_write(mailfd,mailbuff,length);
    
    Close(mailfd);

    return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));

}
extern int s_listfs(struct list_t * ulist_node,struct mail_head * user_md)
{
    int offset;             /*offset of the record's location in sendbuff*/
    int tmpuid;               /*save the user's friends userid templately*/
    int recordfd;                      /*file id of the user records file*/
    struct u_account * frnd_ptr;/*save the struct u_account load by usrid*/
    char sendbuff[FRNDNUM*NAMELEN*2];

    bzero(sendbuff,sizeof(sendbuff));   /*clear the send buffer.*/

    if((recordfd = open(USERECORDS,O_RDONLY)) < 0){
        perror("open user records");    /*open errored,set 0 as no frends*/
        user_md->mailsize = -OPFAILD;
        return s_write(ulist_node->user_connfd,user_md,sizeof(user_md));
    }

    offset = 0; /*load user struct by friends usernumber.max frndnum - 32*/
    for(int i=0;i < FRNDNUM;i++){
        if(((tmpuid = user_st->userfrnd[i]) == -1) || 
        ((frnd_ptr = load_user(_uno_to_cno(tmpuid))) == NULL))continue;
        sprintf(&sendbuff[offset],"%s:%10d\n",
            frnd_ptr->username,frnd_ptr->usernumb);/*print name and num-*/
        offset += strlen(frnd_ptr->username) + 12; /*-ber to send buffer*/
        free(frnd_ptr);
    }

    user_md->mailsize = offset;/*mailsize tell client how many B to recv*/
    if(!s_write(ulist_node->user_connfd,user_md,sizeof(*user_md)))
        return UNCONN;
    if(offset > 0)
        if(s_write(ulist_node->user_connfd,sendbuff,offset) == UNCONN)
        return UNCONN;    /*send return UNCONN,represents client closed.*/

    return 1; 
}
extern int s_sersms(struct list_t * ulist_node,struct mail_head * user_md)
{   
    int mailfd;
    int readlen;
    char mailname[256];
    char sendbuff[4096];

    bzero(mailname,256);
    sprintf(mailname,"%s/%s/%s", /*consisted of .mail/user/sender/file*/
    MAILBOXPTH,user_st->username,user_md->fliename);
    user_md->mailsize = OPSUCED;
    if((mailfd = open(mailname,O_RDONLY)) < 0){    /*error open the mail.*/
        perror("Server's mail");       /*print details of the open error.*/
        if(errno == ENOENT)user_md->mailsize = -NOSUCHF;
        return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
    }

    bzero(sendbuff,4096);
    while((readlen = read(mailfd,sendbuff,4096)) > 0){  /*read from mail.*/
        user_md->mailsize = readlen;/*tell client how many bytes to recv.*/
        if(s_write(ulist_node->user_connfd,user_md,sizeof(*user_md)) 
            == UNCONN)return UNCONN;  /*client has closed the connection.*/
        if(s_write(ulist_node->user_connfd,sendbuff,readlen) == UNCONN)
            return UNCONN;
        bzero(sendbuff,4096);
    }

    /*read over,maybe error or end of file,set flag to tell user file end*/
    user_md->mailsize = (errno == EISDIR) ? -ERRDIRT : OPSUCED;
    Close(mailfd);

    //if(user_md->mailsize == OPSUCED)unlink(mailname);

    return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));

}
extern int s_logout(struct list_t * ulist_node,struct mail_head * user_md)
{
    __rmv_node(&__user_list,ulist_node);/*close socket,remove node from l*/
}
extern int s_others(struct list_t * ulist_node,struct mail_head * user_md)
{
    printf("Others\n\r");
    user_md->mailsize = OPSUCED;/*sign of operate success and do nothing*/
    return s_write(ulist_node->user_connfd,user_md,sizeof(*user_md));
}
extern int 
(*s_logctl(struct mail_head * mhead))(struct list_t *,struct mail_head *)
{   
//  /*after the client connext to server,he should sign in to do mails  */
//  switch(mhead->usercomd){    /*chose a func according to user command*/
//      case Regist:return s_regist;/*user request is register,s_regist */
//      case Logind:return s_logind;   /*user request if login,s_logind */
//      default : return s_others;/*other requests is not permited now, */
//  }    /* because the user is have not sign in to the mail system now */
    if(strcasecmp(mhead->usercomd,REGIST) == 0)return s_regist; /*regist*/
    else if(!strcasecmp(mhead->usercomd,LOGIND))return s_logind;/*logind*/
    else return s_others;

}
extern int 
(*s_servic(struct mail_head * mhead))(struct list_t *,struct mail_head *)
{   /*after user sign in to the system,he can chose the func by command */
//  switch(mhead->usercomd){/*the command from user client,give a func  */
//      case Mailto:return s_mailto;/*user request is mailto-send a mail*/
//      case Listms:return s_listms;/*user request is listms-list mails */
//      case Recvms:return s_recvms;/*user request is recvms-recv a mail*/
//      case Passwd:return s_passwd;/*user request is passwd-changepassd*/
//      case Chname:return s_chname;/*user request is chname-change name*/
//      case Adfrnd:return s_adfrnd;/*user request is adfrnd-add friend */
//      case Rmfrnd:return s_rmfend;/*user request is rmfrnd-remove frnd*/
//      case Seting:return s_seting;/*user request is seting-change sets*/
//      default : return s_others;  /*user request is a illegal function*/
//  }
    if(strcasecmp(mhead->usercomd,MAILTO) == 0)return s_mailto;
    else if(!strcasecmp(mhead->usercomd,LISTMS))return s_listms;/*listms*/
    else if(!strcasecmp(mhead->usercomd,RECVMS))return s_recvms;/*recvms*/
    else if(!strcasecmp(mhead->usercomd,PASSWD))return s_passwd;/*passwd*/
    else if(!strcasecmp(mhead->usercomd,CHNAME))return s_chname;/*chname*/
    else if(!strcasecmp(mhead->usercomd,ADFRND))return s_adfrnd;/*adfrnd*/
    else if(!strcasecmp(mhead->usercomd,RMFRND))return s_rmfend;/*rmfrnd*/
    else if(!strcasecmp(mhead->usercomd,SETING))return s_seting;/*seting*/
    else if(!strcasecmp(mhead->usercomd,LISTFS))return s_listfs;/*listfs*/
    else if(!strcasecmp(mhead->usercomd,ACCEPT))return s_accept;/*accept*/
    else if(!strcasecmp(mhead->usercomd,SERSMS))return s_sersms;/*sersms*/
    else return s_others;
}
extern void __add_node(struct list_t **list_tail,struct list_t * new_node)
{
    /*add this user node to the list,lock the list first,exit if failed.*/
    if(pthread_mutex_lock(&__list_lock))Error("Lock on list",-1);
    if((*list_tail) != NULL)
        (*list_tail)->next = new_node,new_node->prev = *list_tail;
    else __user_list = new_node;
    (*list_tail) = new_node;      /*update the pointer of the list tail.*/
    /*unlock the mutex,print the message and exit when failed to unlock.*/
    if(pthread_mutex_unlock(&__list_lock))Error("Unlocklist",-1);
}
static void __rmv_node(struct list_t **list_head,struct list_t * rmd_node)
{
    if(pthread_mutex_lock(&__list_lock))Error("Lock on list",-1);
    if(*list_head == rmd_node){/*this node is the head of the user list.*/
        if(rmd_node->next)*list_head = rmd_node->next;
        else *list_head = __list_tail = NULL; /*it is only node in list.*/
    }
    else if(rmd_node->prev){
        (rmd_node->prev)->next = rmd_node->next;
        if(!rmd_node->next)__list_tail = rmd_node->prev;/* tail of list.*/
    }
    if(pthread_mutex_unlock(&__list_lock))Error("Unlocklist",-1);/*unlck*/
    
    Close(rmd_node->user_connfd);   /*close the connected socket file id*/
    free(rmd_node->u_acnt);    /*free struct u_account and struct list_t*/
    free(rmd_node);

}
static int __check_frnd(struct list_t * u_node,struct mail_head * user_md)
{
    for(int i=0;i < FRNDNUM;i++)
        if(u_node->u_acnt->userfrnd[i] == user_md->usernumb)return 0;
    return -1;
}
static int __record_user(struct u_account * u_acnt,Numb_t usrnumb)
{
    int result;
    int userecord_fd;               /*save file id of user records file.*/

    result = -1;    /*result of operation,set -1 as operate not success.*/

    if((userecord_fd = open(USERECORDS,O_RDWR)) < 0){/*open for writing.*/
        perror("Open record user");          
        return result;
    }

    //printf("c_no:%d\n\r",_uno_to_cno(usrnumb));
    if(lseek(userecord_fd,(_uno_to_cno(usrnumb))*ACNTSIZE,SEEK_SET)==-1)
        goto end;    /*failed to seek,return -1 as operation was failed.*/

    if(write(userecord_fd,u_acnt,ACNTSIZE) == ACNTSIZE)result = 0;

end:
    perror("__record_user");     /*show result of record user operation.*/
    Close(userecord_fd);
    return result;

}
