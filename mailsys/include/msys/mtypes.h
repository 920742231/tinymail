/*some definition and consts in system*/
#ifndef _MSYS_MTYPES_H
#define _MSYS_MTYPES_H
#include<errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
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
typedef int  Nums_t;

#ifndef NULL
#define NULL    ((void*)0)
#endif

#endif