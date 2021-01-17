    #include<stdio.h>           /*perror:show error mesages */
    #include<string.h>         /*bzero,strcmp,strcpy,strlen */
    #include<unistd.h> /*read , write ,lseek,file operation */
    #include<fcntl.h>          /*open , close ,file control */
    #include<muser/user.h>      /*struct list_t,USERRECORDS */
    #include<muser/account.h>   /*ACNTSIZE,struct u_account */
    #include<msys/normal.h>         /*some normal functions */
    #define     O_FLAGS         (O_RDWR | O_CREAT | O_APPEND)
extern struct u_account * load_user(int c_no)
{
    //printf("u_no:%d\n\r",c_no);
    int retval;  /*save the temp value the function return  */
    int usercord_fd;   /*the file id when open user record  */
    struct u_account * u_acnt;/*save the user record loaded */
    /*open recording file to read user account message      */
    usercord_fd = open(USERECORDS,O_RDONLY | O_CREAT);
    if(usercord_fd < 0)     /*error opening the record file */
    {   /*show details about error, let administrator chose */
        perror("open users records");/*if the problem is im-*/
        return NULL;/*-portant to shutdowm the server system*/
    }
    /*malloc error,system error,we chose to return NULL and */
    /*do not shutdown the server                            */
    if((u_acnt = (struct u_account*)malloc(ACNTSIZE)) == NULL)
        return NULL;
    /*locate the file pointer according to the user number  */
    /*each user has a record in cording file,and the user   */
    /*number represents this user record's location in file */
    retval = lseek(usercord_fd,c_no * ACNTSIZE,SEEK_SET);
    /*lseek error,maybe system errror,maybe no such long of-*/
    /*-fset but it is not neccessary to shutdown the server,*/
    /*then showerror message and return NULL as no such user*/
    if(retval == -1)
        goto err_load;
    /*error read,return NULL and not shutdown the server    */
    if((retval = read(usercord_fd,u_acnt,ACNTSIZE)) != ACNTSIZE)
        goto err_load;
    /*succeed in loading the user account message,return it */
    return u_acnt;

    /*something wrong when loading user record,show details */
    /*and close the file,return NULL as no such user,       */
err_load:
    free(u_acnt);
    perror("load users records");/*message about errordetail*/
    Close(usercord_fd);            /*close user record file */
    return NULL;    /*return NULL as have's found such user */
}
extern int record_user(struct u_account * user_st)
{
    int retval;  /*save the temp value the function return  */
    int user_number;/*new user's account number,return value*/
    int usercord_fd;   /*the file id when open user record  */
    struct u_account u_acnt;/*the last user record,get numb */
    
    /*open recording file to write user account message     */
    /*return NULL if open error instead of shutdown server  */
    usercord_fd = open(USERECORDS,O_FLAGS,0644);
    if(usercord_fd < 0)     /*error opening the record file */
    {   /*show details about error, let administrator chose */
        perror("open users records");/*if the problem is im-*/
        return -1;/*-portant to shutdown the server system. */
    }
    /*multithreading will cause conflict when writing same  */
    /*file,so use pthread mutex to lock the record zone     */
    /*if error locking,must shut the system,otherwise others*/
    /*may can not user the mutex,the server will stop work  */ 
    if(pthread_mutex_lock(&__load_lock))Error("mutex lock",-1);
    /*the file is null,no user in this system now,create new*/
    if(!read(usercord_fd,&u_acnt,ACNTSIZE) && !errno){
        user_st->usernumb = user_number = _cno_to_uno(0);
        if((u_mailbox(user_st) == -1) ||/*recordcreatmailbox*/
        (write(usercord_fd,user_st,ACNTSIZE) != ACNTSIZE))
            goto failure;
        else goto success;       /*return to tell the result*/
    }
    /*seek to the last record,get the user number in it,and */
    /*the number of the new user record is one bigger the it*/
    if(lseek(usercord_fd,-ACNTSIZE,SEEK_END) == -1)
        goto failure;   /*show details about the seek error */
    retval = read(usercord_fd,&u_acnt,ACNTSIZE);
    if(retval != ACNTSIZE)
        goto failure;   /*show details about the read error */
    
    //printf("%d\n\r",u_acnt.usernumb);
    user_st->usernumb = user_number = u_acnt.usernumb + 1;
    if((u_mailbox(user_st) == -1) ||/*failed tocreat mailbox*/
        (write(usercord_fd,user_st,ACNTSIZE) != ACNTSIZE))
        goto failure;   /*error write,show details about it */
    else goto success;/*succeed in recording new user,return*/
failure:
    perror("record new user");/*show message of error detail*/
    user_number = -1;/*set usernumber -1 as failed to record*/
success:
    /*unlock the pthread mutex,shutdown server when error,  */
    /*as other threads cannot get mutex when error unlock it*/
    if(pthread_mutex_unlock(&__load_lock))Error("unlock",-1);
    Close(usercord_fd);/*close file,show details when error */
    return user_number;  /*return new user's account number */
}
extern void _u_list_free(struct list_t * list_head)
{
    struct list_t * list_node;     /* temp node of the list */
loop:
    if(list_head == NULL)return;   /*end of the list,return */
    list_node = list_head;         /* save the current node */
    Close(list_head->user_connfd);  /*close connect socket  */
    list_head = list_head->next; /*turn to next node of list*/
    free(list_node->u_acnt);/*inner pointer(struct u_account*/
    free(list_node);               /*free current list node */
    goto loop;                 /*loop until the list is null*/
}
/*create a directory by username as mailbox.containing mails*/
extern int u_mailbox(struct u_account * user_st)
{
    char pathname[64];  /*name of user mailbox,use username.*/
    bzero(pathname,64);
    sprintf(pathname,"%s/%s",MAILBOXPTH,user_st->username);
    return mkdir(pathname,0700);/*return the result of mkdir*/
}
