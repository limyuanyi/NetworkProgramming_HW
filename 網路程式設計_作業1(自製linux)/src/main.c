#include "../include/config.h"

command_t* parser(char* commandStr);
void exeBuildIn(command_t* cmd);

int main() {
    char commandStr[256]={0};
    char initial_path[100];

    getcwd(initial_path,100);
    strcat(initial_path,"/bin:.");
    setenv("PATH",(initial_path),1);
    char *p=getenv("PATH");
    int n=-1;
    
    while (strcmp(commandStr, "quit") != 0){
        // int fd_out = open("output.txt",O_TRUNC);
        // int fd_in = open("input.txt",O_TRUNC);

        char path[100];
        getcwd(path,100);
        char now[200] = "[Shell ";
        strcat(now,path);
        strcat(now," }%% ");
        printf("\033[35m%s\033[0m",now);

        command_t *cmd1= (command_t*)malloc(sizeof(command_t));
        fgets(commandStr, sizeof(commandStr), stdin);
        commandStr[strlen(commandStr)-1] = '\0';

        //前綴0
        char* start=commandStr;
        while(isspace(*start)){
            start++;
        }
        if(start[0]==0)continue;
        cmd1 = parser(start);
        
        // execflag returns
        //-1 if there is no command
        //0 it is a buildin command
        //1 if it is a simple command
        //2 if it is including a pipe
        //3 if it is including a num pipe

        char temp[256] ={0};
        command_t *cmd2=(command_t*)malloc(sizeof(command_t));
        char* ptr;
        
        if(n==0 &&cmd1->exeflag!=-1){
            file_to_exe(cmd1);
            n--;
            continue;
        }
        switch(cmd1->exeflag){
            case -1:
                printf("Unknown command: [%s].\n",cmd1->command);
                n++;
                break;
            case 0:
                exeBuildIn(cmd1);
                break;
            case 1:
                exeCommand(cmd1,p);
                break;
            case 2:
                //要拆分cmd
                splitCommand(cmd1,temp);
                cmd2=parser(temp);
                exePipe(cmd1,cmd2);
                break;    
            case 3:
                ptr = strchr(cmd1->parameter,'|');
                n = *(ptr+1) - '0';
                exeNumPipe(cmd1);
                break;
        }
        if(cmd1->exeflag==2){
            FILE* file = fopen("output.txt","r");
            char buffer[256];
            while(fgets(buffer,sizeof(buffer),file)!=NULL){
                printf("%s",buffer);
            }
            fclose(file);
        }
        n--;
        free(cmd1);
    };
}