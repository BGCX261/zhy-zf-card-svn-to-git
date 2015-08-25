#include "fcgi_config.h"
#include <stdlib.h>
#include "fcgi_stdio.h"

int main ()
{
    int count = 0;
    while (FCGI_Accept() >= 0) {
        FCGI_printf("Status: 200 OK \r\n\r\n");
        char * query = getenv( "QUERY_STRING" );
        if (query != NULL) {
           FCGI_printf("{%s}", query);
       
           // 解析得到各个参数值

           // 建立数据库连接

           // 从数据库中查询用户的验证信息

           // 判断是否是合法用户
        }
    } /* while */

    return 0;
}
