/*!
******************************************************************************************************************************
@file   http_response_r.h
@brief  http响应消息基类(多线程)
@author 张欢引
$Log: http_response_r.h,v $
Revision 1.0  2012/09/19  http
*******************************************************************************************************************************
*/

#ifndef _HTTP_RESPONSE_R_H__
#define _HTTP_RESPONSE_R_H__

#include "url_utils.h"
#include "http_request_r.h"

class HttpResponseX
{
public:
    HttpResponseX(HttpRequestX * request);
    virtual ~HttpResponseX();

    /**
     * 初始化HttpResponse
     */
    int Init(void);

    /**
     * 设置回应报文的头部。
     */
    int SetHeader(const char *header_name, const char *header_value);
    /**
     * 写应答数据到HTTP输出流。
     */
    int Write(const char *text, ...);

    /**
     * 往HTTP流写数据。
     *
     * 返回 成功写入的字节数
     */
    size_t WriteByte(const void *buf, size_t buf_size);

    /**
     * 直接发送错误码给客户端。
     */
    int SendError(int status);

    /**
     * 302重定向
     */
    int SendRedirection(const char* url);

    int Printf(const char * format, ...);
protected:
    /**
     * 发送设置好的HTTP头部。
     */
    int SendHeader();
protected:
    HttpResponseX(const HttpResponseX &hr);
    HttpResponseX & operator=(const HttpResponseX &hr);
    HttpRequestX * request_;
    param_pair_t * m_headers;              // HTTP响应头部信息数组
    int m_header_quantity;                 // 当前的HTTP头容量。
    int m_header_count;                    // 当前HTTP头个数
    bool m_header_sended;                  // 是否已经送出了HTTP头部, 如果已发出，则不允许在修改HTTP头。
};

#endif  // _HTTP_RESPONSE_R_H__
