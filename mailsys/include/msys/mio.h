#ifndef _MSYS_MIO_H
#define _MSYS_MIO_H

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>

extern size_t 
s_fread(void * buff,size_t buffsize,size_t count,FILE * fp);
extern size_t 
s_fwrite(void * buff,size_t buffsize,size_t count,FILE * fp);
extern int s_write(int fd,void * buff,size_t bufflen);
extern int s_readn(int fd,void * buff,size_t bufflen);

#endif