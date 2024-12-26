#include "../include/config.h"
#include "mysql/mysql.h"

void finish(MYSQL *con) {
    mysql_close(con);
    exit(1);
}

void insertSQL(){

  while(1){
    char user[20];
    char password[20];
    printf("your user name: ");
    scanf("%s",user);
    printf("your password: ");
    scanf("%s",password); 
    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con==NULL) finish(con); 
    
    if (mysql_real_connect(con, "127.0.0.1", "np2023_user", "123456", "np2023",0,NULL,0)==NULL ) finish(con);
    
    char sql[100];
    //printf("%s %s\n",name,password);

    sprintf(sql, "INSERT INTO user (name, password) VALUES ('%s','%s');", user,password);
    //printf("%s\n",sql); 
    mysql_query(con, sql);
    int err_code = mysql_errno(con);

    if(err_code ==1062){
      printf("User name already exist !\n");
    }else{
      break;
    }
  }
}
