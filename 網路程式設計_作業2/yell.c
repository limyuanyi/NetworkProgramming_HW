#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char userFile[20] = "/tmp/userlist";  
void main(int argc, char **argv)
{
    FILE *file = fopen(userFile, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    } 

    pid_t myPid = getppid();
    int myId;

    char line1[256];
    while (fgets(line1, sizeof(line1), file)){
        // fprintf(stderr, "%s",line);
        int uid,port,pid;
        char ip[60],name[50];
        // 解析文件中的每一行
        if (sscanf(line1, "%d %s %s %d %d", &uid, name, ip, &port, &pid)) {
            if(pid==myPid){
                myId=uid;
                break;
            }
        }
    }
    rewind(file);
    char *new_content = malloc(strlen(argv[1]+3));
    sprintf(new_content, "%c%c %s",'y',myId+'0', argv[1]);
    char line[256];
    while (fgets(line, sizeof(line), file)){
        // fprintf(stderr, "%s",line);
        int uid,port,pid;
        char ip[60],name[50];
        // 解析文件中的每一行
        if (sscanf(line, "%d %s %s %d %d", &uid, name, ip, &port, &pid)) {
            // fprintf(stderr, "Parsed: %d %s %s %d %d\n", uid, name, ip, port, pid);
            char fifo_name[128];
            snprintf(fifo_name, sizeof(fifo_name), "/tmp/fifo_%d", port);
            int fd = open(fifo_name, O_WRONLY);
            write(fd,new_content, sizeof(char) * (strlen(new_content) + 1));
            close(fd);
        }
    }
}

