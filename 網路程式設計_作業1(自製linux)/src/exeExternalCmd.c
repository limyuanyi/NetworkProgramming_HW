#include "../include/config.h"


void isExternalCommand(command_t* cmd){
    char *p=getenv("PATH");
    char* dir = strtok(p,":");
    char full_path[256];
    while(dir != NULL){
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd->command);
        if (access(full_path, F_OK) == 0) {
            cmd->exeflag = 1; 
        }
        dir = strtok(NULL, ":");
    }
    isPipeCommand(cmd);

}

void mysh_launch(command_t* cmd, char* full_path){
    char* args[] = {cmd->command, (cmd->parameter[0] == '\0')? NULL : cmd->parameter, NULL};
    pid_t pid,wpid;
    pid = fork();
    if(pid == 0){
        execvp(full_path,args);
        exit(EXIT_FAILURE);
    }else{
            wpid = waitpid(pid,NULL,0);
    }
}

void exeCommand(command_t* cmd , char* p){
    char* dir = strtok(p,":");
    char full_path[256];
    while(dir != NULL){
           snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd->command);

        // 检查文件是否存在
        if (access(full_path, F_OK) == 0) {
            // 检查文件是否可执行
            if (access(full_path, X_OK) == 0) {
                mysh_launch(cmd,full_path);
                return;
            } else {
                printf("File exists but is not executable: %s\n", full_path);
            }
            return;
        }else{
            printf("Unknown command: [%s].\n",cmd->command);
        }

        // 尝试下一个目录
        dir = strtok(NULL, ":");
    }
}
