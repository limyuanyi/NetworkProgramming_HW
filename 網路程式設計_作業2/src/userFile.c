#include "../include/config.h"

void removeUserFile(){
    const char *filepath = "/tmp/userlist";
    remove(filepath);
}


void registerUser(const char *ip,int port,int pid){
//在/temp/内建立一個userList
//存 0 no_name 127.0.0.1 40714 920
    int id = -1;
    printf("buf len=%ld\n",strlen(ip));
    const char *filepath = "/tmp/userlist";
    FILE *fp = fopen(filepath,"a+");
    if(fp == NULL){
        perror("Error opening userlist file");
        return;
    }
    int temp[10] = {0};
    fseek(fp,0,SEEK_END);
    long size = ftell(fp);
    if(size==0)id = 0;//解決file内沒内容
    else{
        char line[256];
        rewind(fp);
        while (fgets(line, sizeof(line), fp)){
            int uid,port,pid;
            char ip[60],name[50];
            // 解析文件中的每一行
            if (sscanf(line, "%d %s %s %d %d", &uid, name, ip, &port, &pid)) {
                temp[uid] = 1;
            }
        }
    }

    for(int i=0;i<10;i++){
        if(temp[i]==0){
            id = i;
            break;
        }
    }

    fprintf(fp,"%d no_name %s %d %d\n",id,ip,port,pid);
    fclose(fp);
}

//加一個黨有人退出了，要刪除它的資料
void deleteUser(int myPORT){
    char *userFile = "/tmp/userlist";
    char *tempFile = "/tmp/userFile"; // 临时文件路径
    FILE *file = fopen(userFile, "r");
    FILE *temp = fopen(tempFile, "w");

    // 逐行读取文件
    char line[256];
    while (fgets(line, sizeof(line), file)){
        // fprintf(stderr, "%s",line);
        int uid,port,pid;
        char ip[60],name[50];
        // 解析文件中的每一行
        if (sscanf(line, "%d %s %s %d %d", &uid, name, ip, &port, &pid)) {
            if(myPORT==port)continue;
            fputs(line,temp);
        }
    }
    fclose(file);
    fclose(temp);
    remove(userFile);
    rename(tempFile,userFile);
}