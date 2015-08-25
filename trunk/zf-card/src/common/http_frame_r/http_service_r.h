/*!
******************************************************************************************************************************
@file   http_service_r.h
@brief  http服务基类(多线程)
@author 张欢引
$Log: http_service_r.h,v $
Revision 1.0  2012/09/19   http
*******************************************************************************************************************************
*/

#ifndef _HTTP_SERVICE_R_H__
#define _HTTP_SERVICE_R_H__

#include <string.h>
#include <vector>
using namespace std;
#include <regex.h>
#include <kiev/rpc_log.h>
#include <kiev/rpc_thread.h>
#include "configurator.h"

class HttpHandlerX;
class HttpServiceX;

class HttpWorkThread : public meizu_rpc::ThreadWorker
{
public:
    HttpWorkThread();
    virtual ~HttpWorkThread();

    int Start(HttpServiceX * service);
    virtual void Run();

    // 初始化各线程的资源
    virtual int Init() = 0;
    virtual int AddHandler() = 0;
    virtual void Stop() { }
protected:
    int AddHandler(const char * path_regex, HttpHandlerX * handler);
protected:
    HttpServiceX * service_;
    vector<HttpHandlerX *> vs_handler_;       // 保存的handler
    meizu_rpc::Thread thread_;
};

class HttpServiceX
{
protected:
    const static uint16_t default_thread_pool_size = 10;
public:
    HttpServiceX(const char * config_file);
    virtual ~HttpServiceX();

    void Start(void);
    void Stop(void);

    void PthreadJoin(pthread_t thread_t) { vs_child_thread_.push_back(thread_t); }

    /*
     * @brief 初始化进程内所有线程可共用资源
     */
    virtual int Init() = 0;
    virtual HttpWorkThread * NewWorkThread() = 0;
    virtual void doStop() = 0;
protected:
    uint16_t thread_pool_size_;
    string config_file_;
    vector<pthread_t> vs_child_thread_;
};

#endif  // _HTTP_SERVICE_R_H__
