/*!
******************************************************************************************************************************
@file   http_handler_r.h
@brief  http消息处理基类(多线程)
@author 张欢引
$Log: http_handler_r.h,v $
Revision 1.0  2012/09/19  http
*******************************************************************************************************************************
*/

#ifndef _HTTP_HANDLER_R_H__
#define _HTTP_HANDLER_R_H__

#include "configurator.h"
#include "http_request_r.h"
#include "http_response_r.h"
#include "http_service_r.h"
#include <regex.h>

class HttpHandlerX
{
public:
    HttpHandlerX(HttpServiceX * service, HttpWorkThread * thread) :
        service_(service), work_thread_(thread)
    {
        memset(path, 0, sizeof(path ) );
    }
    virtual ~HttpHandlerX(){ }

    /*
     * 初始化
     */
    virtual int Init() = 0;

    /**
     * 每次请求过来，都会执行该函数，处理请求，并生成结果写入response
     */
    virtual void DoService(HttpRequestX & request, HttpResponseX & response) = 0;

    /**
     * 仅服务结束时执行一次
     */
    virtual void Stop(void) = 0;

    bool Matches(char * url_path)
    {
        if(url_path == NULL){
            return false;
        }

        if(regexec(&m_compiled_path_regex, url_path, 0, NULL, 0) == 0) {
            return true;
        }
        return false;
    }

    inline const char * GetPathRegex()const
    {
        return path;
    }

    const static int MAX_PATH_REGEX_LEN = 256;
    inline bool InitRegex(const char * path_rgx)
    {
        if (regcomp(&m_compiled_path_regex, path_rgx, 0) != 0) {
            return false;
        }
        strncpy(path, path_rgx, MAX_PATH_REGEX_LEN);
        path[MAX_PATH_REGEX_LEN - 1] = '\0';
        return true;
    }
protected:
    HttpServiceX * service_;
    HttpWorkThread * work_thread_;
    regex_t m_compiled_path_regex;
    char path[256];
};

#endif  // _HTTP_HANDLER_R_H__
