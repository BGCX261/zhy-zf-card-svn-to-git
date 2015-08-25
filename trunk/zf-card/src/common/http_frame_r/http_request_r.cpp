#include "http_request_r.h"
#include "url_utils.h"
#include <stdio.h>

HttpRequestX::HttpRequestX() {
    m_initialized = false;
    m_params = NULL;
    m_param_count = 0;
    m_param_array_size = 0;
    m_method = HTTP_METHOD_ERROR;
    m_schema = SCHEMA_UNKNOW;
}

HttpRequestX::~HttpRequestX()
{
    if(m_params != NULL){
        for(int i=0;i>m_param_count; i++){
            if(m_params[i].name){
                delete m_params[i].name;
            }
            if(m_params[i].value){
                delete m_params[i].value;
            }
        }
        delete m_params;
    }
}

void HttpRequestX::Init(void)
{
    m_method = HTTP_METHOD_ERROR;
    m_initialized = false;
    m_param_count = 0;
    m_schema = SCHEMA_UNKNOW;
}

void HttpRequestX::Destroy(void)
{
    if(m_params != NULL) {
        for(int i = 0;i < m_param_count; i++){
            if(m_params[i].name) {
                delete m_params[i].name;
                m_params[i].name = NULL;
            }
            if(m_params[i].value) {
                delete m_params[i].value;
                m_params[i].value = NULL;
            }
        }
    }
}

const char * HttpRequestX::GetParameter(const char * param_name)
{
    if(param_name == NULL) {
        return NULL;
    }
    if(!m_initialized) {  // 未初始化参数表，读取参数列表
        m_param_count = ParseUrlQuery();        // 解析参数
    }
    if(m_param_count > 0) {
        if(m_params == NULL) {
            return NULL;
        }
        int i = 0;
        while(i < m_param_count){
            if((m_params[i]).name == NULL){
                break;
            }
            if(strcmp(param_name, (m_params[i]).name)==0){
                return m_params[i].value;
            }
            i++;
        }
        return NULL;
    }
    else{
        return NULL;
    }
}

int HttpRequestX::GetParameterCount(void) {
    if (!m_initialized) {
        m_param_count = ParseUrlQuery();     // 解析参数
    }
    return m_param_count;
}

int HttpRequestX::ParseUrlQuery(){
    if(m_initialized){
        return m_param_count;
    }

    m_initialized = true;

    char * content_type = GetContentType();
    if(content_type != NULL && strncmp(content_type, "multipart", 10) == 0) {
        return 0;  //  todo: support multipart/form-data
    }

    char * param_buffer = NULL;
    if(GetMethod() == HTTP_METHOD_POST) {
        unsigned long content_len;
        char * content_len_str = GetEnv("CONTENT_LENGTH");
        sscanf(content_len_str,"%ld",&content_len);
        content_len += 1;
        if(content_len > MAX_CONTENT_LENGTH) {
            // TODO 检查content_len是否超出最大
            return -2;
        }

        param_buffer = (char*) malloc (sizeof(char)*content_len);
        bzero(param_buffer, sizeof(char)*content_len);
        int read_num = FCGX_GetStr(param_buffer, content_len, request_.in);
        if (read_num < 0) {
            return -1;
        }
    }
    else if(GetMethod() == HTTP_METHOD_GET) {
        char * query_string = GetEnv("QUERY_STRING");
        param_buffer = strdup(query_string);
    }
    else {
        //TODO: 不支持的方法
        return -1;
    }

    char *query = param_buffer;
    int len = strlen(param_buffer);
    int num = 0;  // 参数个数
    char * q = query;

    if(q == NULL)
    {
        free(param_buffer);
        return 0;
    }

    while(strsep(&q, "&") != NULL){
        num ++;
    }

    if(num == 0) {
        free(param_buffer);
        return 0;
    }

    int size = (num)*sizeof(param_pair_t);
    if(m_params == NULL){
        m_params = (param_pair_t*)malloc(size);
    }
    else if(m_param_array_size<num){
        // 与上次请求的参数个数相比较，如果需要更多内存，则重新分配
        m_params = (param_pair_t*)realloc(m_params, size);
        m_param_array_size = num;
    }

    if(m_params == NULL)
    {
        free(param_buffer);
        return -1;
    }

    memset(m_params, 0, size);
    param_pair_t * my_param = m_params;

    char * name;
    char * value;
    for (q = query; q < (query + len);){
        name = q;
        value = q;
        q += strlen(q)+1;              // 跳到下一个参数
        name = strsep(&value, "=");    // 分解当前参数
        my_param->name = url_decode2(strdup(name));
        my_param->value = url_decode2(strdup(value==NULL ? "" : value) );
        my_param++;
    }
    free(param_buffer);
    return num;
}

void HttpRequestX::AddParam(const char *name, const char *value){
    m_initialized = true;
    if(m_param_count >= m_param_array_size) {
        if(m_param_array_size < 3){
            m_param_array_size = m_param_array_size + 3;
        }else{
            m_param_array_size = m_param_array_size + m_param_array_size / 3;
        }
        m_params = (param_pair_t*)realloc(m_params, sizeof(param_pair_t) * (m_param_array_size) );
    }
    param_pair_t * my_param = m_params + m_param_count;
    my_param->name = strdup(name);
    my_param->value = strdup(value);
    m_param_count++;
}

