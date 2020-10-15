#include "cloneresource.h"

CloneResource::CloneResource() : Http::Resource("/Clone")
{ // 服务启动时 就加载了滴
}

CloneResource::~CloneResource()
{
}

void CloneResource::Test_Html_Page(Http::Response& response)
{
    response.setStatusCode(Http::Ok);
    response.setHeader(Http::ContentType, "application/json");//application/json  |  text/html
    response.setHeader(Http::ContentLength, "*");
    response.setHeader(Http::Authorization, "*");
    response.setHeader("Access-Control-Allow-Origin", "*");//Access-Control-Allow-Origin | Access-Control-Allow-Headers
    response.setHeader(Http::Accept, "*");
    response.setHeader(Http::Allow, "*");
    response.setBody("{\"ppppp\":\"aaaa\"}");
}

void CloneResource::deliver(const Http::Request& request, Http::Response& response)
{
    GitCmdHandle::getInstence()->setResponse(&response);
    // 读取请求数据
    QByteArray jsonByteContent = request.body();//"{\"project_name\":\"my_project\",\"username\":\"zdc\",\"password\":\"123456\",\"fileinfo\":[{\"filename\":\"/cc/aa.txt\"},{\"filename\":\"/cc/ba.txt\"}]}";
    QString jsonContent = QUrl::fromPercentEncoding(jsonByteContent);
    qDebug() << jsonContent;
    JsonDataParse::getInstence()->startParseJson(jsonContent); // 解析json数据
    //GitApiHandle::getInstence()->gitClone(JsonDataParse::getInstence()->getValue("_UserName"),
    //                                      JsonDataParse::getInstence()->getValue("_Password"),
    //                                      JsonDataParse::getInstence()->getValue("_CloneFromRemote"),
    //                                      JsonDataParse::getInstence()->getValue("_ProjectName"));
    GspClone* obj = new GspClone;
    obj->Clone(JsonDataParse::getInstence()->getValue("_UserName"),
               JsonDataParse::getInstence()->getValue("_Password"),
               JsonDataParse::getInstence()->getValue("_CloneFromRemote"),
               JsonDataParse::getInstence()->getValue("_ProjectName"));
    delete obj;
    // 写数据到客户端
    // In a real world application, one would interpret the request
    // and deliver a proper response.
    //Test_Html_Page(response);
}
