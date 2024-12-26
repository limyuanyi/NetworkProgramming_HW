#include "../include/config.h"

command_t* parser(char* commandStr){
    command_t* cmd = (command_t*)malloc(sizeof(command_t));
    sscanf(commandStr, "%s", cmd->command);
    int x = strlen(cmd->command) + 1;
    sscanf(commandStr + x, "%[^\n]", cmd->parameter);
    if(x>strlen(commandStr)){
        cmd->parameter[0]='\0';
    }
    
    int idx = 0;
    while(isspace(cmd->parameter[idx])){
        idx ++;
    }
    strcpy(cmd->parameter,cmd->parameter+idx);
    cmd->exeflag = -1;
    isBuildIn(cmd);
    isExternalCommand(cmd);
    return cmd;
}
