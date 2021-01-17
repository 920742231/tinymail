#include<stdio.h>
#include<termios.h>
#include<fcntl.h>
void set_noecho()
{
    struct termios ttystate;
    tcgetattr(0,&ttystate);
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_lflag &= ~ECHO;
    ttystate.c_cc[VMIN] = 1;
    tcsetattr(0,TCSANOW,&ttystate);
}
void tty_mode(int op)
{
    static struct termios oldstat;
    static int oldflags;
    if(op == 0)
    {
        tcgetattr(0,&oldstat);
        oldflags = fcntl(0,F_GETFL);
    }
    else
    {
        tcsetattr(0,TCSANOW,&oldstat);
        fcntl(0,F_SETFL,oldflags);
    }
    
}