#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char userFile[20] = "/tmp/userlist";  
char tempFile[20] = "/tmp/userFile";
void main(int argc, char **argv)
{

    FILE *file = fopen(userFile, "r");
    FILE *temp = fopen(tempFile, "w");

    char *nm = malloc(strlen(argv[1])-2);
    sprintf(nm, "%s",argv[1]);
    for (int i = 0; nm[i] != '\0'; i++) {
        if (nm[i] == 13) {  // ASCII 13 是 Carriage Return
            nm[i] = '\0';  // 直接將其替換為結束符
        }
    }

    pid_t myPid = getppid();

    char line[256];
    while (fgets(line, sizeof(line), file)){
        int uid,port,pid;
        char ip[60],name[50];
        // 解析文件中的每一行
        if (sscanf(line, "%d %s %s %d %d", &uid, name, ip, &port, &pid)) {
            if(strcmp(nm,name)==0){
                fclose(file);
                fclose(temp);
                printf("User %s already exists !\n",nm);
                return;
            }
            if(myPid==pid){
                fprintf(temp,"%d %s %s %d %d\n",uid,argv[1],ip,port,pid);
                continue;
            }
            fputs(line,temp);
        }
    }
    fclose(file);
    fclose(temp);
    remove(userFile);
    rename(tempFile,userFile);
    printf("name change accept!\n");
}

