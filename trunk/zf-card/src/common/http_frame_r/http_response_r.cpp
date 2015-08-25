#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <fcgiapp.h>
#include "http_response_r.h"
//#include <fcgi_stdio.h>

HttpResponseX::HttpResponseX(HttpRequestX * request) :
        request_(request),
        m_headers(NULL),
        m_header_quantity(0),
        m_header_count(0),
        m_header_sended(false)
{
}

HttpResponseX::~HttpResponseX() {
    delete m_headers;
}

int HttpResponseX::Init(void){
    m_header_sended = false;
    if(m_header_quantity==0)
    {
        // 初始化10个header
        m_header_quantity = 10;
        m_headers = (param_pair_t*)malloc(sizeof(param_pair_t)*m_header_quantity);
    }
    m_header_count = 0;
    if(m_headers!=NULL)
    {
        memset(m_headers, 0, sizeof(param_pair_t)* m_header_quantity);
        return 0;
    }
    return -1;
}

int HttpResponseX::Write(const char * fmt, ...)
{
    if(!m_header_sended) {
        SendHeader();
    }
    va_list args;
    va_start(args, fmt);
    int count = FCGX_VFPrintF(request_->GetFCGX_Request()->out, fmt, args);
    va_end(args);
    return count;
}

size_t HttpResponseX::WriteByte(const void *buf, size_t buf_size){
    if(!m_header_sended){
        SendHeader();
    }
    return FCGX_PutStr((const char *)buf, (int)buf_size, request_->GetFCGX_Request()->out );
}

int HttpResponseX::Printf(const char * format, ...)
{
    int result;
    va_list args;
    va_start(args, format);
    result = FCGX_VFPrintF(request_->GetFCGX_Request()->out, format, args);
    va_end(args);
    return result;
}

int HttpResponseX::SetHeader(const char *header_name, const char *header_value){
    if(m_header_sended){
        return -1; // 已发送
    }
    assert(m_headers != NULL && header_name != NULL && header_value != NULL);

    // 增加header的容量
    if(m_header_count >= m_header_quantity){
        param_pair_t * new_headers = (param_pair_t*) realloc(m_headers, sizeof(param_pair_t) * (m_header_quantity + 3));
        if(new_headers == NULL){
            return -2;
        }
        m_header_quantity += 3;
        m_headers = new_headers;
    }

    // 未检查header_name是否重复
    m_headers[m_header_count].name = const_cast<char*>(header_name);
    m_headers[m_header_count].value = const_cast<char*>(header_value);
    ++m_header_count;
    return 0;
}

int HttpResponseX::SendHeader(){
    m_header_sended = true;
    param_pair_t* param = m_headers;
    int count = 0;
    int sended_count = 0;
    for(int i = 0;i< m_header_count; i++) {
        sended_count = Printf("%s:%s\n", param->name, param->value);
        if(sended_count<0){
            return -1;
        }
        param++;
        count += sended_count;
    }

    if(sended_count<0)  {
        return -1;
    }
    if(sended_count==0){
        Printf("%s:%s\n", "Content-Type", "text/html;charset=utf8");
    }

    sended_count = Printf("\n");
    return count + sended_count;
}

int HttpResponseX::SendError(int status){
    if(m_header_sended){
        return -1;//头部已经发送，则报错。
    }
    switch(status){
    case 401:
        Printf("Status: 401 Unauthorized\r\n\r\n"
                "<html><head><title>401 Unauthorized</title></head><body>401 Unauthorized</body></html>");
        break;
    case 403:
        Printf("Status: 403 Forbidden\r\n\r\n"
                "<html><head><title>403 Forbidden</title></head><body>403 Forbidden</body></html>");
        break;
    case 404:
        Printf("Status: 404 Not Found\r\n\r\n"
                "<html><head><title>404 Not Found</title></head><body>404 Not Found</body></html>");
        break;
    case 405:
        Printf("Status: 405 Method Not Allowed\r\n\r\n"
                "<html><head><title>405 Method Not Allowed</title></head><body>405 Method Not Allowed</body></html>");
        break;
    case 414:
        Printf("Status: 414 Request-URI Too Long\r\n\r\n"
                "<html><head><title>414 Request-URI Too Lon</title></head><body>414 Request-URI Too Lon</body></html>");
        break;
    case 400:
    default:
        Printf("Status: 400 Bad Request\r\n\r\n"
                "<html><head><title>400 Bad Request</title></head><body>400 Bad Request</body></html>");
        break;
    }
    return 0;
}

int HttpResponseX::SendRedirection(const char* url){
    if(m_header_sended){
        return -1;  //  头部已经发送，则报错。
    }
    SetHeader("Location", url);
    Printf("Status: 302 Found\r\n\r\n"
           "<html><head><title>302 Found</title></head><body>302 Found<br><a href=\"%s\">%s</a></body></html>", url, url);
    return 0;
}

