#include "checkoutresource.h"

CheckoutResource::CheckoutResource() : Http::Resource("/Checkout")
{

}
CheckoutResource::~CheckoutResource()
{

}

void CheckoutResource::deliver(const Http::Request &request, Http::Response &response)
{
    GitCmdHandle::getInstence()->setResponse(&response);
    // 读取请求数据
    QByteArray jsonByteContent = request.body();
    QString jsonContent = QUrl::fromPercentEncoding(jsonByteContent);
    qDebug() << jsonContent;
    JsonDataParse::getInstence()->startParseJson(jsonContent); // 解析json数据
    //GitApiHandle::getInstence()->gitCheckout(JsonDataParse::getInstence()->getValue("_UserName"),
    //                                         JsonDataParse::getInstence()->getValue("_Password"),
    //                                         JsonDataParse::getInstence()->getValue("_ProjectName"),
    //                                         JsonDataParse::getInstence()->getValue("_CheckoutVersion"),
    //                                         JsonDataParse::getInstence()->getFileList());
    GspCheckout* obj = new GspCheckout;
    obj->CheckoutVersion(JsonDataParse::getInstence()->getValue("_UserName"),
                         JsonDataParse::getInstence()->getValue("_Password"),
                         JsonDataParse::getInstence()->getValue("_ProjectName"),
                         JsonDataParse::getInstence()->getValue("_CheckoutVersion"),
                         JsonDataParse::getInstence()->getFileList());
    delete obj;
    // 写数据到客户端
    // In a real world application, one would interpret the request
    // and deliver a proper response.
}
