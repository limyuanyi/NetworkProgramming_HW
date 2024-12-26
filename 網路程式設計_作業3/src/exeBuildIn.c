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
    printf("***WELCOME TO SHELL HELP***\r\n");
    printf("-Use the shell at your own risk...\r\n");
    printf("\r\n*cd\r\n*pwd\r\n*printenv\r\n*setenv\r\n*quit\r\n");
    printf("\r\n");
}

void my_cd(command_t* cmd){
    if(cmd->parameter == NULL){
        perror("Mysh error at cd, lack of args\r\n");
    }
    else{
        if(chdir(cmd->parameter) != 0)
            perror("Mysh error at chdir\r\n");
    }
    char path[100];
    getcwd(path,100);
    setenv("PWD",path,1);
}

void my_pwd(command_t* cmd){
    char path[100];
    getcwd(path,100);
    printf("%s\r\n",path);
}

void my_printenv(command_t* cmd){
    char *path=getenv(cmd->parameter);
    if(path != NULL){
        printf("PATH：%s\r\n",path);
    }
    else{
        printf("The path environment variable is not set.\r\n");
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
    printf("Thank you for using!\r\n");
    return;
}

void exeBuildIn(command_t* cmd){
    for(int i=0; i<my_buildIn_nums(); i++){
        if(strcmp(cmd->command,buildIn_cmd[i])==0){
            (buildIn_func[i])(cmd);
        }
    }
}
