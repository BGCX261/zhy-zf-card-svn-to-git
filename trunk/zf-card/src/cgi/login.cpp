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
       
       // �����õ���������ֵ

       // �������ݿ�����

       // �����ݿ��в�ѯ�û�����֤��Ϣ

       // �ж��Ƿ��ǺϷ��û�
    }
    return 0;
}
