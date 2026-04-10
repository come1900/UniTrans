#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define MOTOR_MAGIC 'L'
#define RELAY1 		_IOW(MOTOR_MAGIC, 0,int)
#define RELAY2 		_IOW(MOTOR_MAGIC, 1,int)

int main(int argc, char **argv)
{
    int fd;
    unsigned int retval;
    char cmd;
    char info;


    //输入的命令格式为:./relay_test  relay1\relay2 0\1     relay1表示控制j20继电器工作,relay2表示控制
    //j21继电器工作
    //0表示断开电路,1表示连通电路
    if(argc != 3)
    {
        fprintf(stderr, "Usage: relay_test relay1|relay2  0|1\n");
        exit(1);
    }
    //sscanf(argv[1], "%c", &cmd);
    sscanf(argv[2], "%d", &info);

    fd = open("/dev/relay", O_RDWR);
    if (fd < 0)
    {
        perror("open device relay error");
        exit(1);
    }

    if(strcmp(argv[1],"relay1") == 0)
    {

        retval = ioctl(fd, RELAY1,info);
    }
    else if(strcmp(argv[1],"relay2") == 0)
    {

        retval = ioctl(fd, RELAY2,info);
    }
    else
    {
        printf("printf relay1 or relay2!\n");
        exit(-1);
    }
    if(retval==-1)
    {
        perror("ioctl error\n");
        exit(-1);
    }

    close(fd);
    return 0;
}
