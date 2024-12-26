#include "../include/config.h"

void my_help();
void my_cd(command_t* cmd);
void my_pwd(command_t* cmd);
void my_printenv(command_t* cmd);
void my_setenv(command_t* cmd);
void my_quit();

char *buildIn_cmd[] = {
    "help",
    "cd",
    "pwd",
    "printenv",
    "setenv",
    "quit"
};

void (*buildIn_func[])(command_t* cmd) = {
    &my_help,
    &my_cd,
    &my_pwd,
    &my_printenv,
    &my_setenv,
    &my_quit

};

int my_buildIn_nums(){
    return sizeof(buildIn_cmd)/sizeof(buildIn_cmd[0]);
}

void isBuildIn(command_t* cmd){
    int num_bulidin_cmd = sizeof(buildIn_cmd)/sizeof(buildIn_cmd[0]);

    for(int i=0;i<num_bulidin_cmd;i++){
        if(strcmp(cmd->command,buildIn_cmd[i])==0){
            cmd->exeflag = 0;
            return ;
        }
    }
}


void my_help(){
    printf("***WELCOME TO SHELL HELP***\n");
    printf("-Use the shell at your own risk...\n");
    printf("\n*cd\n*pwd\n*printenv\n*setenv\n*quit\n");
    printf("\n");
}

void my_cd(command_t* cmd){
    if(cmd->parameter == NULL){
        perror("Mysh error at cd, lack of args\n");
    }
    else{
        if(chdir(cmd->parameter) != 0)
            perror("Mysh error at chdir\n");
    }
    char path[100];
    getcwd(path,100);
    setenv("PWD",path,1);
}

void my_pwd(command_t* cmd){
    char path[100];
    getcwd(path,100);
    printf("%s\n",path);
}

void my_printenv(command_t* cmd){
    char *path=getenv(cmd->parameter);
    if(path != NULL){
        printf("PATH：%s\n",path);
    }
    else{
        printf("The path environment variable is not set.\n");
    }
}

void my_setenv(command_t* cmd){
    char *temp = cmd->parameter;
    sscanf(cmd->parameter, "%s", temp);
    int x = strlen(temp) + 1;
    char *addr = cmd->parameter + x;
    setenv(temp,addr,1);
}

void my_quit(){
    printf("Thank you for using!\n");
    return;
}

void exeBuildIn(command_t* cmd){
    for(int i=0; i<my_buildIn_nums(); i++){
        if(strcmp(cmd->command,buildIn_cmd[i])==0){
            (buildIn_func[i])(cmd);
        }
    }
}


// pid == 0：這個值表示當前的進程是子進程。
//pid > 0：這個值表示當前的進程是父進程，而 pid 的具體值是子進程的 PID。
//pid == -1：這表示創建子進程失敗，通常是由於資源不足或超出系統限制。

//檔案描述符
//dup(),dup2()
//進程
//pid_t,fork()
//execv,execvp(),....各種開始新進程s