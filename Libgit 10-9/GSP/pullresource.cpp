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

//    //获取文件状态
//    GitApiHandle::getInstence()->checkFileStatus(JsonDataParse::getInstence()->getValue("_ProjectName"),
//                                           JsonDataParse::getInstence()->getValue("_UserName"),
//                                           JsonDataParse::getInstence()->getValue("_Password"));
//    QStringList fileList ; //需要提交的文件
//    foreach (FileStatus fS, GitApiHandle::getInstence()->m_FileList) {
//        if(!fS.status.contains("deleted file")) {
//            qDebug() << "fileName: "<<fS.file;
//            qDebug() << "fileStatus: "<< fS.status;
//            fileList.append(fS.file);
//        }

//    }
//    //拉取之前先做一个提交到本地的操作
//    GspCommit *obj = new GspCommit;
//    obj->CommitFileToLocal(JsonDataParse::getInstence()->getValue("_ProjectName"),
//                            JsonDataParse::getInstence()->getValue("_UserName"),
//                            JsonDataParse::getInstence()->getValue("_Password"),
//                            JsonDataParse::getInstence()->getValue("_CommitEmail"),
//                            nullptr,
//                            fileList);

    GitApiHandle::getInstence()->gitPull_2(JsonDataParse::getInstence()->getValue("_ProjectName"),
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
