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

int main(int argc, char **argv)
{
    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con==NULL) finish(con);    
    if (mysql_real_connect(con, "127.0.0.1", "np2023_user", "123456", "np2023",0,NULL,0)==NULL ) finish(con);
    
    argv[1][strlen(argv[1])-1]='\0';

    char sql_search[256];
    sprintf(sql_search,"SELECT * FROM mailBox where id = '%s';",argv[1]);
    //printf("%s\n",sql_search);

    if(mysql_query(con, sql_search))finish(con);
        
    MYSQL_RES *result = mysql_store_result(con);
    int num_rows = mysql_num_rows(result);
    if(num_rows==0){
        printf("Mail id unexist!\n");
    }else{
        char sql_delete[256];
        sprintf(sql_delete,"DELETE FROM mailBox where id = '%s';",argv[1]);
        //printf("%s\n",sql_delete);    
        if(mysql_query(con, sql_delete))finish(con);
    }

    mysql_free_result(result);
    mysql_close(con);
}

// gcc delMail.c -o ./bin/delMail -L/usr/lib/x86_64-linux-gnu -lmysqlclient -I/usr/include/mysql