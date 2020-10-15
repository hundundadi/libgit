#include "pullresource.h"

PullResource::PullResource() : Http::Resource("/Pull")
{

}

PullResource::~PullResource()
{

}

void PullResource::deliver(const Http::Request &request, Http::Response &response)
{
    GitCmdHandle::getInstence()->setResponse(&response);
    // 读取请求数据
    QByteArray jsonByteContent = request.body();
    QString jsonContent = QUrl::fromPercentEncoding(jsonByteContent);
    qDebug() << jsonContent;
    JsonDataParse::getInstence()->startParseJson(jsonContent); // 解析json数据
    GitApiHandle::getInstence()->gitPull(JsonDataParse::getInstence()->getValue("_ProjectName"),
                                         JsonDataParse::getInstence()->getValue("_UserName"),
                                         JsonDataParse::getInstence()->getValue("_Password"),
                                         JsonDataParse::getInstence()->getValue("_CommitEmail"),
                                         JsonDataParse::getInstence()->getFileList(),
                                         true);
    //GspPull* pullObj = new GspPull;
    //pullObj->Pull(JsonDataParse::getInstence()->getValue("_ProjectName"),
    //              JsonDataParse::getInstence()->getValue("_UserName"),
    //              JsonDataParse::getInstence()->getValue("_Password"),
    //              JsonDataParse::getInstence()->getValue("_CommitEmail"),
    //              JsonDataParse::getInstence()->getValue("_CloneFromRemote"),
    //              true);
    //delete pullObj;
    // 写数据到客户端
    // In a real world application, one would interpret the request
    // and deliver a proper response.
}
