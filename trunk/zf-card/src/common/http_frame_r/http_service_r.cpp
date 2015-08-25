#include "http_service_r.h"
#include "http_handler_r.h"
// HttpWorkThread virtual int Init() = 0;
// HttpWorkThread virtual int AddHandler() = 0;

HttpWorkThread::HttpWorkThread() :
                  thread_(this)
{
}

HttpWorkThread::~HttpWorkThread()
{
}

int HttpWorkThread::Start(HttpServiceX * service)
{
    service_ = service;

    // 对线程对象进行初始化
    int iRet = Init();
    if (0 != iRet)
    {
        LOGERR("HttpWorkThread::Start::Init() failed. Ret:%d", iRet);
        return iRet;
    }

    // 调用线程对象的AddHandler函数
    iRet = AddHandler();
    if (0 != iRet)
    {
        LOGERR("HttpWorkThread::Start::AddHandler failed. Ret:%d", iRet);
        return iRet;
    }

    // 启用另一个线程
    thread_.Start();
    service_->PthreadJoin(thread_.get_pthread_t() );
    return 0;
}

void HttpWorkThread::Run()
{
    HttpRequestX request;
    HttpResponseX response(&request);
    int loop_count = 0;

    FCGX_InitRequest(request.GetFCGX_Request(), 0, 0);
    while(1)
    {
        int iRet = FCGX_Accept_r(request.GetFCGX_Request() ); // >= 0
        if (iRet < 0)
        {
            LOGINFO("FCGX_Accept_r return %d", iRet);
            break;
        }
        bool matched = false;
        request.Init();
        if(response.Init() != 0) {
            LOGERR("response init error");
            break;
        }

        // 依次匹配各个handler
        for (size_t i = 0; i < vs_handler_.size(); i++)
        {
            HttpHandlerX * handler = vs_handler_[i];
            if (handler->Matches(request.GetScriptName()) ) {
                handler->DoService(request, response);
                matched = true;
                break;
            }
        }

        if(!matched) {
            LOGERR("Request URL not found: %s", request.GetScriptName());
            response.SendError(404);
        }

        request.Destroy();
        FCGX_Finish_r( request.GetFCGX_Request() );
        loop_count++;

        //if(loop_count > m_max_loop_count){
        //    break;
        //}
    }

    // 依次释放各handler的资源

    // 释放线程对应的资源
}

int HttpWorkThread::AddHandler(const char * path_regex, HttpHandlerX * handler)
{
    if(path_regex == NULL || handler == NULL || strlen(path_regex) > HttpRequestX::MAX_CONTENT_LENGTH)  {
         LOGERR("HttpWorkThread::AddHandler failed");
         return -1;
     }

    int iRet = handler->Init();
    if (iRet != 0)
    {
        LOGERR("HttpWorkThread::AddHandler %s failed, Ret:%d", path_regex, iRet);
        return iRet;
    }

    if (!handler->InitRegex(path_regex) )
    {
        LOGERR("handler->InitRegex(%s) failed", path_regex);
        return -2;
    }

    vs_handler_.push_back(handler);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HttpServiceX::HttpServiceX(const char * config_file) :
                 thread_pool_size_(default_thread_pool_size)
{
    config_file_ = config_file;
}

HttpServiceX::~HttpServiceX()
{
}

void HttpServiceX::Start(void)
{
    int iRet = Init();
    if (0 != iRet) {
        LOGERR("HttpServiceX::Start::Init() failed. Ret:%d", iRet);
        return;
    }

    // 创建线程
    FCGX_Init();
    for (uint16_t i = 0; i < thread_pool_size_; i++)
    {
        HttpWorkThread * workThread = NewWorkThread();
        if (NULL == workThread)
        {
            LOGERR("NewWorkThread return NULL");
            break;
        }

        iRet = workThread->Start(this);
        if (0 != iRet)
        {
            LOGERR("workThread->Start failed, Ret:%d", iRet);
            continue;
        }
    }

    // 等待所有子线程退出
    for (uint32_t i = 0; i < vs_child_thread_.size(); ++i)
    {
        pthread_join(vs_child_thread_[i], NULL);
    }
}

void HttpServiceX::Stop(void)
{
    doStop();
}

