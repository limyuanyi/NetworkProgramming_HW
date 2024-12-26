#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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


    char sql_search[256];
    sprintf(sql_search,"select * from mailBox where receiver = '%s';",me);
    //printf("%s\n",sql_search);
    // printf("%s\n",sql_search);

    if(mysql_query(con, sql_search))finish(con);

    MYSQL_RES *result = mysql_store_result(con);
    int num_rows = mysql_num_rows(result);
    if(num_rows==0){
        printf("Empty !\n");
    }else{
        MYSQL_ROW row;
        printf("<ID>\t<date>\t\t\t<sender>\t<message>\n\r");
        while(row=mysql_fetch_row(result)){
            printf("%s\t%s\t%s\t\t%s\n\r",row[0],row[1],row[2],row[3]);
        }
    }
    mysql_free_result(result);
    mysql_close(con);
}

// gcc listMail.c -o ./bin/listMail -L/usr/lib/x86_64-linux-gnu -lmysqlclient -I/usr/include/mysql