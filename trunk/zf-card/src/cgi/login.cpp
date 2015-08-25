#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main()
{
    printf ("Status: 200 OK \r\n\r\n");
    char * query = getenv( "QUERY_STRING" );
    if (query != NULL) {
       printf("{%s}", query);
       
       // 解析得到各个参数值

       // 建立数据库连接

       // 从数据库中查询用户的验证信息

       // 判断是否是合法用户
    }
    return 0;
}
