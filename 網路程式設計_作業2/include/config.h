#ifndef CONFIG_H
#define CONFIG_H

#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<unistd.h>

#include<stdint.h>
#include<signal.h>
#include<sys/socket.h>
#include<arpa/inet.h>

typedef struct commandType {
    char command[100];
    char parameter[100];
    int exeflag;
}command_t;


#endif
