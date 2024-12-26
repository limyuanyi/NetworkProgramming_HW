#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mysql.h>

void finish(MYSQL *con) {
    printf("finish\n");
    printf("%d\n",-1);
    mysql_close(con);
    exit(1);
}

char userFile[20] = "/tmp/userlist";  


int main(int argc, char **argv)
{
    char me[50];
    
    FILE *file = fopen(userFile, "r");
    pid_t myPid = getppid();

    char line[256];
    while (fgets(line, sizeof(line), file)){
        int uid,port,pid;
        char ip[60],name[50];
        // 解析文件中的每一行
        if (sscanf(line, "%d %s %s %d %d", &uid, name, ip, &port, &pid)) {
            if(myPid==pid){
                strcpy(me,name);
                break;
            }
        }
    }
    fclose(file);

    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con==NULL) finish(con);    
    if (mysql_real_connect(con, "127.0.0.1", "np2023_user", "123456", "np2023",0,NULL,0)==NULL ) finish(con);
    
    argv[1][strlen(argv[1])-1]='\0';
    char sql_search[256];
    sprintf(sql_search,"SELECT * FROM allGroup WHERE groupName = '%s';",argv[1]);
    // printf("%s\n",sql_search);

    if(mysql_query(con, sql_search))finish(con);

    MYSQL_RES *result = mysql_store_result(con);
    int num_rows = mysql_num_rows(result);

    if(num_rows>0){
        printf("Group already exist !\n");
    }else{
        char sql_insert[256];
        sprintf(sql_insert,"INSERT INTO allGroup(groupName,createBy) VALUES('%s','%s');",argv[1],me);
        if(mysql_query(con, sql_insert))finish(con);
        printf("Create Success !");

        char sql_create[256];
        sprintf(sql_create,"CREATE TABLE `%s`(user VARCHAR(255));",argv[1]);
        if(mysql_query(con, sql_create))finish(con);
        // printf("%s\n",sql_create);

        char sql_insert1[256];
        sprintf(sql_insert1,"INSERT INTO `%s` values('%s');",argv[1],me);
        // printf("%s\n",sql_insert1);
        if(mysql_query(con, sql_insert1))finish(con);
    }

    mysql_free_result(result);
    mysql_close(con);
}

//gcc createGroup.c -o ./bin/createGroup -L/usr/lib/x86_64-linux-gnu -lmysqlclient -I/usr/include/mysql