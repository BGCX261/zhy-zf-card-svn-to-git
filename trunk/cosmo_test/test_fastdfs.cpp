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
using namespace std;

int main(int argc, char ** args)
{
	if (argc < 4)
	{
		printf("usage %s fdfs_conf_file log4c_conf upload_file \n ", args[0] );
		return -1;
	}

	char * fdfs_conf_file = args[1];
	char * log4c_conf = args[2];
	// char * upload_file = args[3];

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
	
	// 获得tracker连接 
	TrackerServerInfo trackerServer_; 
	ret = tracker_get_connection_r(&trackerServer_); 
	if (0 != ret || trackerServer_.sock < 0) {
		log_root.error("tracker_get_connection_r failed, ret:%d \n", ret);
		return ret;
	}
	
	// 获得storage 
	int store_path_index = 0;
	TrackerServerInfo * storage_ = new TrackerServerInfo(); 
	tracker_query_storage_store(&trackerServer_, storage_, &store_path_index);
	
	// 或 tracker_query_storage_update(&trackerServer_, ptmpStorage, group_name.c_str(), file_name.c_str() ); 
	
	// 写入 
	// 从文件中读取得到数据
	char content[1024] = {0};    // 从upload_file文件中读到的数据
	snprintf(content, sizeof(content), "1234567890");
	long content_len = 10;
	char file_id[256] = {0};      //
	// storage_upload_by_callback1(&trackerServer_, storage_, store_path_index, cb, content, (long) content_len, NULL, NULL, 0, NULL, file_id); 
	int result = storage_upload_appender_by_filebuff1(&trackerServer_, storage_, 0, content, content_len, NULL, NULL, 0, NULL, file_id);   
	if (result != 0) {
		log_root.error("storage_upload_appender_by_filebuff1 failed, ret:%d, content_len:%d \n ", ret, content_len);
		return result;
	}
	
	log_root.info("upload success, content_len:%d, file_id:%s", content_len, file_id );
	
	// 追加 
	//storage_append_by_filebuff1(&trackerServer_, storage_, content, content_len, att.c_str());   
	
	// 下载 
	//storage_download_file_ex1(&trackerServer_, storage_, file_id, file_offset, 0, callback, callback_ex_data, &filesize);   
	//storage_do_download_file1_ex(&trackerServer_, storage_, FDFS_DOWNLOAD_TO_BUFF, att.c_str(), offset, 0, &heapbuf, NULL, &rcontent_len);   
	
	// 删除 
	//storage_delete_file1(&trackerServer_, storage_, att.c_str());   
	
	// 查询信息 
	//storage_query_file_info_ex1(&trackerServer_, storage_, fileid.c_str(), &info, true);   
	
	// 关闭 
	tracker_disconnect_server(storage_); 
	fdfs_quit(&trackerServer_); 
	tracker_disconnect_server( &trackerServer_ );   
	
	delete storage_;
	// 
	tracker_close_all_connections(); 
	fdfs_client_destroy();
	return 0;
}
