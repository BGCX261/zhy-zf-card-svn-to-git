/*!
******************************************************************************************************************************
@file   http_request_r.h
@brief  Http请求类(多线程)
@author 张欢引
$Log: http_request_r.h,v $
Revision 1.0  2012/09/19  http
*******************************************************************************************************************************
*/

#ifndef _HTTP_REQUEST_R_H__
#define _HTTP_REQUEST_R_H__

#include <string.h>
#include <stdlib.h>
#include "url_utils.h"
#include <fcgiapp.h>

// Http请求类。多个访问重用同一个HttpRequest对象。重用该对象之前需要用Init方法做清理。
// 可以用于多线程
// 注：没有从FCGX_Request派生，主要是考虑可能后续会从HttpRequest派生
class HttpRequestX
{
public:
    enum EnumHttpMethod {
        HTTP_METHOD_GET,
        HTTP_METHOD_POST,
        HTTP_METHOD_HEAD,
        HTTP_METHOD_UNKNOW,
        HTTP_METHOD_ERROR,
    };

    enum HttpSchema {
        SCHEMA_HTTP, SCHEMA_HTTPS, SCHEMA_UNKNOW,
    };

    enum HttpConst {
        MAX_CONTENT_LENGTH = 4096*2,
        //最大请求参数长度
    };

    HttpRequestX(void);
    virtual ~HttpRequestX(void);

    void Init(void);
    virtual void Destroy(void);

    /**
     * @brief 查找与参数名匹配的参数。
     */
    const char * GetParameter(const char *param_name);

    int GetParameterCount(void);

    /**
     * 取得请求的HTTP方法
     */
    EnumHttpMethod GetMethod(void) {
        if (m_method == HTTP_METHOD_ERROR) {
            char * httpMethod = GetEnv("REQUEST_METHOD");
            if (strncmp(httpMethod, "POST", 4) == 0) {
                m_method = HTTP_METHOD_POST;
            } else if (strncmp(httpMethod, "GET", 3) == 0) {
                m_method = HTTP_METHOD_GET;
            } else {
                m_method = HTTP_METHOD_UNKNOW;
            }
        }
        return m_method;
    }

    char * GetMethod(void) const {
        return GetEnv("REQUEST_METHOD");
    }

    HttpSchema GetHttpSchema(void) {
        if (m_schema == SCHEMA_UNKNOW) {
            const char* https = GetEnv("HTTPS");
            if (https != NULL && strncmp(https, "on", 2) == 0) {
                m_schema = SCHEMA_HTTPS;
            } else {
                m_schema = SCHEMA_HTTP;
            }
        }
        return m_schema;
    }

    inline char * GetReferer(void) const { return GetEnv("HTTP_REFERER"); }

    inline char * GetUserAgent(void) const { return GetEnv("HTTP_USER_AGENT"); }

    /**
     * The interpreted pathname of the current CGI (relative to the document root)
     */
    inline char * GetScriptName(void) const { return GetEnv("SCRIPT_NAME"); }

    inline char * GetQueryString(void) const { return GetEnv("QUERY_STRING"); }

    inline char * GetRemoteAddr(void) const { return GetEnv("REMOTE_ADDR"); }

    inline char * GetServerName(void) const { return GetEnv("SERVER_NAME"); }

    inline char * GetServerPort(void) const { return GetEnv("SERVER_PORT"); }

    inline char * GetContentType(void) const { return GetEnv("CONTENT_TYPE"); }

    inline char * GetRequestURI(void) const{ return GetEnv("REQUEST_URI"); }

    //char *GetEnvParam(const char *paraname)const{
    //    return getenv(paraname);
    //}

    void AddParam(const char * name, const char * value);

    FCGX_Request * GetFCGX_Request() { return & request_; }

    inline char * GetEnv(const char * env_name) const
    {
        return FCGX_GetParam(env_name, request_.envp);
    }
protected:
    /**
     * @brief: 解析url的GET方法的查询字符串。并且做utf8的decode
     * @return:
     *   参数个数
     */
    int ParseUrlQuery(void);
protected:
    HttpRequestX(const HttpRequestX & hr);
    HttpRequestX & operator=(const HttpRequestX &hr);

    bool m_initialized;        // 已初始化的HTTP请求
    param_pair_t * m_params;    // 参数列表name-value对，真实字符串仍然保存在param_buffer里
    int m_param_count;         // 这次请求的参数个数
    int m_param_array_size;    // 参数数组大小

    EnumHttpMethod m_method;     // 请求的HTTP方法
    HttpSchema m_schema;         // HTTP协议

    FCGX_Request request_;
};

#endif  // _HTTP_REQUEST_R_H__
