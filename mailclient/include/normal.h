#ifndef _NORMAL_H
#define _MORMAL_H

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<fcntl.h>

#ifndef Malloc
#define Malloc(type,name,num) ({       \
name = (type*)malloc(sizeof(type)*num);\
assert(name);})
#endif

#define Error(msg,no) ({perror(msg);exit(no);})

#define Close(fd)   if(close(fd))perror("close");
#define Fclose(fp)  if(fclose(fp))perror("flcose");
#define Pclose(fp)  if(pclose(fp))perror("pclose");

#endif