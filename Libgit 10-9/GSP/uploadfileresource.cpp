#include "uploadfileresource.h"

UpLoadFileResource::UpLoadFileResource() : Http::Resource("/UploadFile")
{

}

UpLoadFileResource::~UpLoadFileResource()
{

}

void UpLoadFileResource::deliver(const Http::Request &request, Http::Response &response)
{
    // 读取请求数据
    QByteArray jsonByteContent = request.body();
    QString jsonContent = QUrl::fromPercentEncoding(jsonByteContent);
    qDebug() << jsonContent;
    JsonDataParse::getInstence()->startParseJson(jsonContent); // 解析json数据

    // 写数据到客户端
    // In a real world application, one would interpret the request
    // and deliver a proper response.
    //ReadFileContent(JsonDataParse::getInstence()->getValue("_ProjectName"),
    //                JsonDataParse::getInstence()->getFileList().first());
    //QString fileJsonData = JsonDataParse::getInstence()->generateUploadFileContentJsonString(m_optFile);
    //ResponseWeb(fileJsonData, response);
    GspUploadFile* upload = new GspUploadFile;
    upload->postFile(JsonDataParse::getInstence()->getValue("_ProjectName"),
                     JsonDataParse::getInstence()->getValue("_FilePostUrl"),
                     JsonDataParse::getInstence()->getFileList().first());
    QString fileJsonData = JsonDataParse::getInstence()->generateUploadFileContentJsonString(upload->m_optFile);
    ResponseWeb(fileJsonData, response);
    delete upload;
}

void UpLoadFileResource::ReadFileContent(QString prjName, QString fileName)
{
    QTextCodec *codec=QTextCodec::codecForName("gbk");
    m_optFile.file = fileName;
    QString tmpLocal = RW_ConfigFile::getInstence()->getRepositoryPath() + prjName + "\\" + fileName;
    QFile optFile(tmpLocal);
    if (!optFile.exists()) {//文件不存在
        m_optFile.retcode = "-1";
        m_optFile.retmsg = "文件不存在！";
        return ;
    }
    if (!optFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_optFile.retcode = "-2";
        m_optFile.retmsg = "文件无法打开！";
        return ;
    }
    QByteArray ba_FileContent = optFile.readAll();
    m_optFile.content = codec->toUnicode(ba_FileContent);
    m_optFile.size = GitApiHandle::getInstence()->caculateSuitFileSizeUnit(optFile.size());
    m_optFile.retmsg = "获取成功！";
    m_optFile.retcode = "0";
    optFile.close();
}

void UpLoadFileResource::ResponseWeb(QString &ContentData, Http::Response &response)
{
    response.setStatusCode(Http::Ok);
    response.setHeader(Http::ContentType, "application/json");
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.setHeader(Http::ContentLength, "*");
    response.setHeader(Http::Authorization, "*");
    response.setHeader(Http::Accept, "*");
    response.setHeader(Http::Allow, "*");
    response.setBody(ContentData.toUtf8());
}
