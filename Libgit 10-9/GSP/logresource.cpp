#include "logresource.h"

LogResource::LogResource() : Http::Resource("/Log")
{

}

LogResource::~LogResource()
{

}

void LogResource::deliver(const Http::Request &request, Http::Response &response)
{
    GitCmdHandle::getInstence()->setResponse(&response);
    // 读取请求数据
    QByteArray jsonByteContent = request.body();
    QString jsonContent = QUrl::fromPercentEncoding(jsonByteContent);
    qDebug() << jsonContent;
    JsonDataParse::getInstence()->startParseJson(jsonContent); // 解析json数据
    //GitApiHandle::getInstence()->gitLog(JsonDataParse::getInstence()->getValue("_ProjectName"),
    //                                    JsonDataParse::getInstence()->getValue("_UserName"),
    //                                    JsonDataParse::getInstence()->getValue("_Password"),
    //                                    JsonDataParse::getInstence()->getFileList());
    GspLog* obj = new GspLog;
    obj->Log(JsonDataParse::getInstence()->getValue("_ProjectName"),
             JsonDataParse::getInstence()->getValue("_UserName"),
             JsonDataParse::getInstence()->getValue("_Password"),
             JsonDataParse::getInstence()->getFileList());
    delete obj;
    // 写数据到客户端
    // In a real world application, one would interpret the request
    // and deliver a proper response.
}
