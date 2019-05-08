#include <stdio.h>

#include <mysql.h>

int main()
{
  MYSQL *mysql  =mysql_init(NULL);
  mysql_real_connect(mysql,"localhost","root","123",)
  return 0;
}
