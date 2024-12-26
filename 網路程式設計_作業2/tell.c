#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char userFile[20] = "/tmp/userlist";  
void main(int argc, char **argv)
{
    int id;
    sscanf(argv[1], "%d", &id);

    
    FILE *file = fopen(userFile, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    } 

    pid_t myPid = getppid();
    int myId;

    char line[256];
    int myPort;
    while (fgets(line, sizeof(line), file)){
        // fprintf(stderr, "%s",line);

        int uid,port,pid;
        char ip[60],name[50];
        // 解析文件中的每一行
        if (sscanf(line, "%d %s %s %d %d", &uid, name, ip, &port, &pid)) {
            // fprintf(stderr, "Parsed: %d %s %s %d %d\n", uid, name, ip, port, pid);
            if (uid == id) {
                myPort = port;
            }

            if(pid == myPid){
                argv[1][0] = uid+'0';
            }
        }
    }

    char fifo_name[128];
    snprintf(fifo_name, sizeof(fifo_name), "/tmp/fifo_%d", myPort);
    // fprintf(stderr,"%s\n",fifo_name);

    int fd = open(fifo_name, O_WRONLY);
    if (fd == -1) {
        perror("Error opening FIFO");
        exit(1);
    }
    // 写入数据
    write(fd, argv[1], sizeof(char) * (strlen(argv[1]) + 1));

    //因爲FIFO會阻塞，所以要server要讀取

    printf("send accept\n");
    close(fd);
}
// Format: tell user_id message
//<user(id) told you>: message
// send accept!‘’‘’‘’‘’‘’‘’‘‘’‘’‘’‘

// FIFO

// 每一個client都是fork出來的孩子（子與子溝通）

//BUG
//關掉第一個port后，另外一個沒關，userlist還存著

