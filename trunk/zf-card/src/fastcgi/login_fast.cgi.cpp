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
       
           // �����õ���������ֵ

           // �������ݿ�����

           // �����ݿ��в�ѯ�û�����֤��Ϣ

           // �ж��Ƿ��ǺϷ��û�
        }
    } /* while */

    return 0;
}
