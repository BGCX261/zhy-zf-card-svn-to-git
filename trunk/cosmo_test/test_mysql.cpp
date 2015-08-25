#include <stdio.h>
#include <stdlib.h>
#include "mysql.h"

int main()
{
	  // ������sock��ͨ��
    MYSQL *sock, mysql;
    
    if (!(sock = mysql_init(&mysql)))  {
        printf("Couldn't initialize mysql structn");
        exit(1);
    }
    
    // mysql_options(sock, MYSQL_READ_DEFAULT_GROUP, "connect");
    char * host = "192.168.1.1";
    char * user = "";
    char * passwd = "";
    char * db = "";
    uint port = 3306;
    
    sock = mysql_real_connect(&mysql, host, user, passwd, db, port, NULL,0);
    if (!sock)
    {
        fprintf(stderr,"Couldn't connect to engine!n%sn",mysql_error(sock));
        exit(1);
    }
    mysql.reconnect = 1;
    
    // �����л���һ�����ݿ�
    if (mysql_select_db(sock, "test"))
    {
        printf("Couldn't select database test: Error: %s ......\n", mysql_error(sock));
        exit(1);
    }
    
    // ����
    char szInsert[512] = {0};
    snprintf(szInsert, sizeof(szInsert), "INSERT INTO () ");
    if(mysql_query(sock, szInsert))  {   // mysql_real_query
        fprintf(stderr,"Query failed (%s)n", mysql_error(sock) );
        exit(1);
    }
    
    // ��ѯ
    char szSelect[512] = {0};
    snprintf(szSelect, sizeof(szSelect), "SELECT * FROM t_Table where ");
    int res = mysql_query(sock, szSelect);
    if (res < 0) {
    	  // ����
    }
    
    MYSQL_RES * result = mysql_store_result(sock);
    MYSQL_ROW row;
    MYSQL_FIELD * field;
    unsigned int row_count= 0;
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        mysql_field_seek(result, 0);
        for(unsigned int i= 0; i < mysql_num_fields(result); i++)
        {
          field = mysql_fetch_field(result);
          // ���Դ�ӡ��field�����ͼ�ֵ
        }
        row_count++;
    }
    mysql_free_result(result);
    
    mysql_close(sock);
    
    return 0;
}


