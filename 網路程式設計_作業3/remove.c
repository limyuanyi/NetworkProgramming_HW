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
    
    char groupName[50];
    char user[100];
    sscanf(argv[1],"%s %[^\n]",groupName,user);
    user[strlen(user)-1]='\0';

    char sql_IsOwner[256];
    sprintf(sql_IsOwner,"SELECT createBy FROM allGroup WHERE groupName = '%s';",groupName);
    // printf("%s\n",sql_IsOwner);
    if(mysql_query(con, sql_IsOwner))finish(con);
    MYSQL_RES *result = mysql_store_result(con);
    int num_rows = mysql_num_rows(result);
    if(num_rows > 0){
        MYSQL_ROW row=mysql_fetch_row(result);
        if(strcmp(row[0],me)==0){
            // printf("%s == %s\n",row[0],me);
        }
        else{
            printf("You don't have permissions !\n");
            mysql_free_result(result);
            mysql_close(con);
            return 0;
        }
    }else{
        printf("Group not found !\n");
        mysql_free_result(result);
        mysql_close(con);
        return 0;
    }


    char deletePerson[10][50];
    const char delim[] = " ";
    int len = 0;
    char *token= strtok(user,delim);
    while(token!=NULL){
        strcpy(deletePerson[len],token);
        len++;
        token = strtok(NULL,delim);
        // printf("%s\n",invitees[len-1]);
    }

    char sql_searchUserValid[256];
    while(--len>=0){
        sprintf(sql_searchUserValid,"SELECT * FROM `%s` where user = '%s';",groupName,deletePerson[len]);
        // printf("%s\n",sql_searchUserValid);
        if(mysql_query(con, sql_searchUserValid))finish(con);
        result = mysql_store_result(con);
        num_rows = mysql_num_rows(result);

        if(num_rows==0){
            char sql_search2[256];
            sprintf(sql_search2,"SELECT * FROM user where name = '%s';",deletePerson[len]);
            // printf("%s\n",sql_search2);            
            if(mysql_query(con, sql_search2))finish(con);

            MYSQL_RES *result1 = mysql_store_result(con);
            int num_rows1 = mysql_num_rows(result1);
            if(num_rows1==0)printf("%s not found !\n",deletePerson[len]);
            else printf("%s is not in group.\n",deletePerson[len]);
        }
        else{
            char sql_delete[256];
            sprintf(sql_delete,"delete from `%s` where user = '%s';",groupName,deletePerson[len]);
            // printf("%s\n",sql_delete);            
            if(mysql_query(con, sql_delete))finish(con);
            printf("%s remove success !\n",deletePerson[len]);
        }
    }
    mysql_free_result(result);
    mysql_close(con);
}

// gcc delMail.c -o ./bin/delMail -L/usr/lib/x86_64-linux-gnu -lmysqlclient -I/usr/include/mysql