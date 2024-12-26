#include "../include/config.h"

command_t* parser(char* commandStr);
void exeCommand(command_t* cmd , char* p);


void isPipeCommand(command_t* cmd){
    
    // normal pipe command
    char *p = strchr(cmd->parameter,'|');
    if(strchr(cmd->parameter,'|')){
        cmd->exeflag = 2;
        if(*(p+1)>='1' && *(p+1)<='9'){
            cmd->exeflag = 3;
        }
    }
}

void splitCommand(command_t* cmd1,char* temp){
    strcpy(temp,cmd1->parameter);

    char* p = strchr(cmd1->parameter,'|');
    int idx = (int)(p - cmd1->parameter);

    if(idx!=0){
        strncpy(cmd1->parameter,cmd1->parameter,idx-1);
        //後綴0
        int end = idx-1;
        while(isspace(cmd1->parameter[end])){
            end--;
        }
        cmd1->parameter[end+1]='\0';
    }else cmd1->parameter[idx] = '\0';

    //cmd2的前綴0
    int idx1 =2;
    while(isspace(cmd1->parameter[idx+idx1])){
        idx1++;
    }
    strcpy(temp,cmd1->parameter+idx+idx1);
    temp[strlen(temp)+1]='\0';
    // printf("cmd1: %s,%s,%d\n",cmd1->command,cmd1->parameter,cmd1->exeflag);
}

void exePipe_to_file(command_t* cmd){
    pid_t pid;
    char *p=getenv("PATH");

    if((pid=fork())==-1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }    
    int fd_in = open("input.txt", O_RDONLY , 0644);
    int fd_out = open("output.txt", O_WRONLY | O_TRUNC, 0644);

    if(pid == 0){
        dup2(fd_in,STDIN_FILENO);
        dup2(fd_out,STDOUT_FILENO);
        exeCommand(cmd,p);
        exit(EXIT_FAILURE);
    }

    waitpid(pid,NULL,0);
}

void cpFile(){
    FILE *fd_out = fopen("output.txt","r");
    FILE *fd_in = fopen("input.txt","w");
    char buffer[256];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fd_out)) > 0) {
        fwrite(buffer, 1, bytes, fd_in);
    }
    fclose(fd_in);
    fclose(fd_out);
}

void exePipe(command_t* cmd1,command_t* cmd2){
    int pipefd[2];
    pid_t pid1,pid2;
    command_t* cmd3 = (command_t*)malloc(sizeof(command_t));

    if((pipe(pipefd))==-1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    if((pid1=fork())==-1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    char *p=getenv("PATH");
    //子進程
    if(pid1==0){
        close(pipefd[0]);
        dup2(pipefd[1],STDOUT_FILENO);
        close(pipefd[1]);

        //執行指令；
        exeCommand(cmd1,p);
        exit(EXIT_FAILURE);
    }

    char temp[256] = {0};
    if(cmd2->exeflag==2){
        splitCommand(cmd2,temp);
    }

    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    //子進程二
    if((pid2=fork())==-1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }
    if(pid2 == 0){
        dup2(pipefd[0],STDIN_FILENO);
        close(pipefd[0]);
        dup2(fd,STDOUT_FILENO);
        close(pipefd[1]);

        //執行命令
        exeCommand(cmd2,p);
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(pid1,NULL,0);
    waitpid(pid2,NULL,0);

    if(cmd2->exeflag==2){
        cmd3 = parser(temp);
        while(cmd3->exeflag==2){
            splitCommand(cmd3,temp);
            cpFile();
            exePipe_to_file(cmd3);
            free(cmd3);
            cmd3 = parser(temp);
        }
        cpFile();
        exePipe_to_file(cmd3);   
    }
}

void exeNumPipe(command_t* cmd){
    char* ptr = strchr(cmd->parameter,'|');
    int idx = (int)(ptr - cmd->parameter);

    if(idx!=0){
        strncpy(cmd->parameter,cmd->parameter,idx-1);
        //後綴0
        int end = idx-1;
        while(isspace(cmd->parameter[end])){
            end--;
        }
        cmd->parameter[end+1]='\0';
    }else cmd->parameter[idx] = '\0';

    pid_t pid;
    char *p=getenv("PATH");
    int fd_out = open("temp.txt", O_WRONLY |O_CREAT| O_TRUNC, 0644);

    if((pid=fork())==-1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }    
    if(pid == 0){
        dup2(fd_out,STDOUT_FILENO);
        exeCommand(cmd,p);
        exit(EXIT_FAILURE);
    }

    waitpid(pid,NULL,0);
}

void file_to_exe(command_t* cmd){
    pid_t pid;
    char *p = getenv("PATH");
    if((pid=fork())==-1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }    
    int fd_temp = open("temp.txt", O_RDONLY , 0644);
    if(pid == 0){
        dup2(fd_temp,STDIN_FILENO);
        exeCommand(cmd,p);
        exit(EXIT_FAILURE);
    }
    waitpid(pid,NULL,0);
}
