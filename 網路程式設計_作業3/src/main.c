#include<stdio.h>
#include <sys/select.h>
#define LISTEN_PORT 6002
#include "../include/config.h"
#define USER_FILE "user_data.txt"


command_t* parser(char* commandStr);
void server(int connfd,struct sockaddr_in *cliaddr);


static void sig_handler(int sig){
    int status;
    if(sig == SIGCHLD){
        int pid = wait(&status);
        fprintf(stderr,"CATCH SIGNAL PID=%d\n",pid);
    }
}

void create_fifo(int myPORT){
    char fifo_name[100];
    snprintf(fifo_name, sizeof(fifo_name), "/tmp/fifo_%d", myPORT);

    // 創建 FIFO 文件
    if (mkfifo(fifo_name, 0666) < 0) {
        perror("mkfifo error");
        exit(1);
    }
    printf("FIFO created: %s\n", fifo_name);  
}

void delete_fifo(int myPORT) {
    char fifo_name[100];
    snprintf(fifo_name, sizeof(fifo_name), "/tmp/fifo_%d", myPORT);

    // 刪除 FIFO 文件
    if (unlink(fifo_name) < 0) {
        perror("unlink error");
    } else {
        printf("FIFO deleted: %s\n", fifo_name);
    }
}

int main(){
    int listenfd ,connfd;
    struct sockaddr_in servaddr,cliaddr;
    char buff[100];

    signal(SIGCHLD, sig_handler);

    if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket error");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(LISTEN_PORT);

    if(bind(listenfd,(struct sockaddr *)&servaddr, sizeof(servaddr))<0){
        perror("bind error");
        exit(1);
    }

    if(listen(listenfd,5)!=0){
        perror("listen error");
        exit(1);
    }

    removeUserFile();

    //client
    while(1){
        int len = sizeof(cliaddr);
        connfd = accept(listenfd,(struct sockaddr *)&cliaddr, &len);
        if(connfd > 0){
            fprintf(stderr,"connection from %s, port %d\n",
                inet_ntop(AF_INET,&cliaddr.sin_addr,buff,sizeof(buff)),
                ntohs(cliaddr.sin_port));
            int child = fork();
            if(child == 0){//小孩
                close(listenfd);
                server(connfd, &cliaddr);
                exit(0);
            } else{//父親
                fprintf(stderr,"server pid %d\n",child);
                  
                registerUser(buff,ntohs(cliaddr.sin_port),child);
            }
            close(connfd); 
        }
    }
}
    
int login(char* login, char* p){
    char* full_path="/home/limyuanyi/411021236_work3/bin/login";
    char* args[] = {"login",login, NULL};
    pid_t pid,wpid;
    pid = fork();
    if(pid == 0){
        execvp(full_path,args);
        exit(EXIT_FAILURE);
    }else if(pid>0){
        wpid = waitpid(pid,NULL,0);
    }
    char res[256];
    FILE *fd_res = fopen("/tmp/resTmp.txt","r");
    fgets(res, sizeof(res), fd_res);
    return res[0]-'0';
}

void name(char* user, char* p){
    char* full_path="/home/limyuanyi/411021236_work3/bin/name";
    char* args[] = {"name",user, NULL};
    pid_t pid,wpid;
    pid = fork();
    if(pid == 0){
        execvp(full_path,args);
        exit(EXIT_FAILURE);
    }else if(pid>0){
        wpid = waitpid(pid,NULL,0);
    }
}

void server(int connfd,struct sockaddr_in *cliaddr){
    command_t *cmd = NULL;
    int myPORT;
    char *pwd;
    char myIP[100];
    inet_ntop(AF_INET,&cliaddr->sin_addr, myIP, sizeof(myIP));
    myPORT = ntohs(cliaddr->sin_port);

    getcwd(pwd, sizeof(pwd));
    dup2(connfd,STDIN_FILENO);
    dup2(connfd, STDOUT_FILENO);
    setbuf(stdout,NULL);
    close(connfd);

    create_fifo(myPORT);

    printf("MyIP=%s,MyPORT=%d\n",myIP, myPORT);

    char commandStr[256]={0};
    char initial_path[100];
    getcwd(initial_path,100);
    strcat(initial_path,"/bin:.");
    setenv("PATH",(initial_path),1);
    char *p=getenv("PATH");
    int n=-1;

    char fifo_path[128];
    snprintf(fifo_path, sizeof(fifo_path), "/tmp/fifo_%d", myPORT);
    int res = 1;

    while(res){
        char user[20];
        char password[20];

        printf("user_name: ");
        scanf("%s",user);
        printf("password: ");
        scanf("%s",password);

        //0 : 正確 ； 1：不正確 ； 2：找不到
        char login_cmd[100];
        sprintf(login_cmd,"%s %s",user,password);
        res = login(login_cmd,p);


        if(res==0){
            name(user,p);
            while(1){
                printf("(%s)%% ",user);
                int fifo_fd = open(fifo_path,O_RDONLY | O_NONBLOCK);

                fd_set read_fds;
                FD_ZERO(&read_fds);

                // 添加标准输入和 FIFO 到监听集合
                FD_SET(STDIN_FILENO, &read_fds); // 标准输入
                FD_SET(fifo_fd, &read_fds);      // FIFO

                int max_fd = (fifo_fd > STDIN_FILENO ? fifo_fd : STDIN_FILENO) + 1;

                //會阻塞等待輸入，不設定等待時間
                int ready = select(max_fd, &read_fds, NULL, NULL, NULL);

                // 处理标准输入
                if (FD_ISSET(STDIN_FILENO, &read_fds)) {
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
                            printf("Unknown command: [%s].\r\n",cmd1->command);
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

                    if(strcmp(commandStr,"quit\r")==0){
                        write(connfd,commandStr,sizeof(commandStr));
                        break;
                    }  
                }
                
                // 处理 FIFO 输入
                if (FD_ISSET(fifo_fd, &read_fds)) {
                    char fifoStr[128];
                    ssize_t bytes_read = read(fifo_fd, fifoStr, sizeof(fifoStr) - 1);

                    //處理tell
                    if (bytes_read > 0) {
                        fifoStr[bytes_read] = '\0';
                        if(fifoStr[0]=='y')
                            printf("<user(%d) yelled>: %s\n",fifoStr[1]-'0',fifoStr+3);
                        else{
                            printf("<user(%d) told you>: %s\n",fifoStr[0]-'0',fifoStr+2);
                        }
                    } else if (bytes_read == 0) {
                        close(fifo_fd);
                        FD_CLR(fifo_fd, &read_fds); // 从集合中移除 FIFO
                    }
                }

            };
        }
        else if(res==1){
            printf("Password error !\n");
        }
        else if(res==2){
            int  again;
            printf("User not found !\n");
            printf("Create account or login again ? <1/2> : ");
            scanf("%d",&again);
            if(again==2){continue;}
            insertSQL();
            printf("Create success !\n");
        }
    }



    delete_fifo(myPORT);
    deleteUser(myPORT);
}



//quit退出client端
//使用send() & read(),把commandStr傳過去，server再strcmp是的話就close()