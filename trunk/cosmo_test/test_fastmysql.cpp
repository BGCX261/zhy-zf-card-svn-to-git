#include "log4cpp/Category.hh"
#include "log4cpp/PropertyConfigurator.hh"
#include <tracker_types.h>
#include <client_func.h>
#include <tracker_proto.h> 
#include <tracker_client.h>
#include <fdfs_global.h>
#include <storage_client1.h>
#include <logger.h>
//#include <tracker_proto.h>
//#include <storage_client.h>

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "mysql.h"
#define TIME_OUT 1
using namespace std;

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        printf("usage %s fdfs_conf_file log4c_conf upload_file \n ", argv[0] );
        return -1;
    }

    char * fdfs_conf_file = argv[1];
    char * log4c_conf = argv[2];
    // char * upload_file = argv[3];

    try  {
            log4cpp::PropertyConfigurator::configure(log4c_conf);
    }
    catch (log4cpp::ConfigureFailure & e)  {
            printf("Configure Problem %s \n ", e.what() );
            return -1;
    }

    log4cpp::Category & log_root = log4cpp::Category::getRoot();
      
    log_init(); 
    log_set_prefix(g_fdfs_base_path, "fdfs"); 
            
    int ret = fdfs_client_init(fdfs_conf_file );
     
    if (ret != 0) {
            log_root.error("fdfs_client_init %s failed, ret:%d ", fdfs_conf_file, ret);
            return ret;
    }

    TrackerServerInfo trackerServer_;
    ret = tracker_get_connection_r(&trackerServer_);
    if (0 != ret || trackerServer_.sock < 0) {
            log_root.error("tracker_get_connection_r failed, ret:%d \n", ret);
            return ret;
    }

    int store_path_index = 0;
    TrackerServerInfo * storage_ = new TrackerServerInfo();
    tracker_query_storage_store(&trackerServer_, storage_, &store_path_index);

    // tracker_query_storage_update(&trackerServer_, ptmpStorage, group_name.c_str(), file_name.c_str() ); 
    char content[1024] = {0};    // 
    snprintf(content, sizeof(content), "1234567890");
    long content_len = 10;
    char file_id[256] = {0};      //
    // storage_upload_by_callback1(&trackerServer_, storage_, store_path_index, cb, content, (long) content_len, NULL, NULL, 0, NULL, file_id); 
    int rsult = storage_upload_appender_by_filebuff1(&trackerServer_, storage_, 0, content, content_len, NULL, NULL, 0, NULL, file_id);
    if (rsult != 0) {
           log_root.error("storage_upload_appender_by_filebuff1 failed, ret:%d, content_len:%d \n ", ret, content_len);
           delete storage_;
           return rsult;
   }

   log_root.info("upload success, content_len:%d, file_id:%s", content_len, file_id );

   //storage_append_by_filebuff1(&trackerServer_, storage_, content, content_len, att.c_str());   

   //storage_download_file_ex1(&trackerServer_, storage_, file_id, file_offset, 0, callback, callback_ex_data, &filesize);   
   //storage_do_download_file1_ex(&trackerServer_, storage_, FDFS_DOWNLOAD_TO_BUFF, att.c_str(), offset, 0, &heapbuf, NULL, &rcontent_len);   

   //storage_delete_file1(&trackerServer_, storage_, att.c_str());   

   //storage_query_file_info_ex1(&trackerServer_, storage_, fileid.c_str(), &info, true);   
   tracker_disconnect_server(storage_);
   fdfs_quit(&trackerServer_);
   tracker_disconnect_server( &trackerServer_ );

   delete storage_;
   // 
   tracker_close_all_connections();
   fdfs_client_destroy();



   MYSQL *sock, mysql;

   if (!(sock = mysql_init(&mysql)))  {
       log_root.error("mysql_init failed, sock:%d", sock);
       exit(1);
   }

   // mysql_options(sock, MYSQL_READ_DEFAULT_GROUP, "connect");
   char * host = "127.0.0.1";
   char * user = "root";
   char * passwd = "123456";
   char * db = "cards";
   uint port = 3306;
   int iTIMEOUT = TIME_OUT;

   if(mysql_options(sock, MYSQL_OPT_CONNECT_TIMEOUT, &iTIMEOUT) != 0)
   {
      log_root.error("mysql_options failed");
      exit(1);
   }
   sock = mysql_real_connect(&mysql, host, user, passwd, db, port, NULL,0);
   if (!sock)
   {
       log_root.error("mysql_real_connect failed, %s",mysql_error(sock));
       exit(1);
   }
   mysql.reconnect = 1;

   if (mysql_select_db(sock, db))
   {
       log_root.error("mysql_select_db failed,error: database %s %s", db, mysql_error(sock));
       exit(1);
   }

   char szInsert[512] = {0};
   snprintf(szInsert, sizeof(szInsert), "INSERT INTO fdfs (file_id, content_len) values ('%s', %ld) ", file_id, content_len);
   if(mysql_query(sock, szInsert))  {   // mysql_real_query
       log_root.error("mysql_query failed,error: %s", mysql_error(sock) );
       exit(1);
   }

   char szSelect[512] = {0};
   snprintf(szSelect, sizeof(szSelect), "SELECT * FROM fdfs");
   int res = mysql_query(sock, szSelect);
   if (res < 0) {
       log_root.error("mysql_query failed, error: %s", mysql_error(sock));
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
		 string field_name = field->name;
		 switch(field->type)
		 {
		 // case MYSQL_TYPE_DECIMAL:
	     // printf("name:%s value:%f \n", field_name.c_str(), atod(row[i]) );
	     //		 break;
		 case MYSQL_TYPE_STRING:
		 case MYSQL_TYPE_VAR_STRING:
			 printf("name:%s value:%s \n", field_name.c_str(), row[i] );
			 break;
		 // case MYSQL_TYPE_LONGLONG:
		 //	 printf("name:%s, value:%ld \n", filed_name.c_str(), strtoll(row[i], NULL, 10) );
		 //	 break;
		 default:
			 printf("error: i:%d, name:%s type:%d \n", i, field_name.c_str(), field->type );
			 break;
		 }
       }
       row_count++;
   }
   mysql_free_result(result);

   mysql_close(sock);
   return 0;
}
