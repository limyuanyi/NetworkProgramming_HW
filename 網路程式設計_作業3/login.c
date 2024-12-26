#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  int res;
  if (con==NULL) finish(con); 
 	
  if (mysql_real_connect(con, "127.0.0.1", "np2023_user", "123456", "np2023",0,NULL,0)==NULL ) finish(con);

  char sql[100];
  char name[20];
  char password[20];
  sscanf(argv[1],"%s %s",name,password);
  //printf("%s %s\n",name,password);
  password[strlen(password)]='\0';
  sprintf(sql, "SELECT * FROM user WHERE name='%s'", name);
  //printf("%s\n",sql); 
  if (mysql_query(con, sql)) finish(con);

  MYSQL_RES *result = mysql_store_result(con);

  if (result==NULL) finish(con);

  int num_rows = mysql_num_rows(result);
  MYSQL_ROW row;
  if (num_rows>0) {
    row=mysql_fetch_row(result);
    //printf("row[2]: %s\r\n",row[1]);
    //printf("%s\n",password);
    if (strcmp(row[1],password)==0)
      res = 0;
    else
      res = 1;
  } else
    res = 2;
  //0 : 正確 ； 1：不正確 ； 2：找不到
  mysql_free_result(result);

  FILE *fd= fopen("/tmp/resTmp.txt","w");
  fprintf(fd,"%d",res);
  fclose(fd);
  mysql_close(con);
  exit(0);
}

//np2023_user 123456
//sudo grep bind-address /etc/mysql/mysql.conf.d/mysqld.cnf 顯示bind-ip
