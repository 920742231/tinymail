#include<errno.h>
#include<unistd.h>
#include<mio.h>
extern size_t 
s_fread(void * buff,size_t buffsize,size_t count,FILE * fp)
{
    size_t rdcount = fread(buff,buffsize,count,fp);
    if(rdcount!=count && !feof(fp))perror("fread");
    return rdcount;
}
extern size_t 
s_fwrite(void * buff,size_t buffsize,size_t count,FILE * fp)
{
    size_t wrcount = fwrite(buff,buffsize,count,fp);
    if(wrcount != count)perror("fwrite");
    return wrcount;
}
extern int s_write(int fd,void * buff,size_t bufflen)
{
    size_t writlen = write(fd,buff,bufflen);
    if(writlen != bufflen)perror("write");
    return writlen;
}
/*read bufflen bytes of data to buff from file fd.   */
extern int s_readn(int fd,void * buff,size_t bufflen)
{
    size_t offset;          /*sum of read bytes      */
    size_t readln;          /*numbers of each read   */
    offset = 0;
    while(bufflen - offset) /*if offset==bufflen stop*/
    {
        readln = read(fd,buff+offset,bufflen-offset);
        if(readln <= 0)     /*read error,print detail*/
        {
        perror("read");     /*perror,print error msg.*/
        return readln;      /*return to tell the err.*/
        }
        offset += readln;   /*sum the read size,     */
    }
    return bufflen;         /*return length that read*/
}