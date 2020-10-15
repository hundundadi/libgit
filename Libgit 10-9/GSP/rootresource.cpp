#include "rootresource.h"

RootResource::RootResource() : Http::Resource("/")
{

}

RootResource::~RootResource()
{

}

void RootResource::deliver(const Http::Request& request, Http::Response& response)
{
    // 读取请求数据
    qDebug() << request.body();
    // 写数据到客户端
    // In a real world application, one would interpret the request
    // and deliver a proper response.
    response.setStatusCode(Http::Ok);
    response.setHeader(Http::ContentType, "text/html");
    response.setBody("<h1>The a Git_client_server!</h1><a href=\"/Clone\">go to second</a>");

}
