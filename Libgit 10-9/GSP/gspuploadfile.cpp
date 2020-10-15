#include "gspuploadfile.h"

GspUploadFile::GspUploadFile(QObject *parent) : QObject(parent)
{

}

QString GspUploadFile::getFile_MEMI_Type(const QString &filePath)
{
    QMimeDatabase db;
    QMimeType subFileMimeType = db.mimeTypeForFile(filePath);
    qDebug() << subFileMimeType.name();
    return subFileMimeType.name();
}

void GspUploadFile::postFile(QString prjName, QString postUrl, QString fileName)
{
    QString tmpLocal = RW_ConfigFile::getInstence()->getRepositoryPath() + prjName + "\\" + fileName;
    QFile optFile(tmpLocal);
    if (!optFile.exists()) {//文件不存在
        m_optFile.retcode = "-1";
        m_optFile.retmsg = "文件不存在！";
        return ;
    }
    if (!optFile.open(QIODevice::ReadOnly)) {
        m_optFile.retcode = "-2";
        m_optFile.retmsg = "文件无法打开！";
        return ;
    }
    QNetworkAccessManager *m_pHttpMgr = new QNetworkAccessManager();
    //设置url
    QString url = QString("%1?_type=file&fileName=%2").arg(postUrl).arg(fileName);
    //设置头信息
    QNetworkRequest requestInfo;
    requestInfo.setUrl(QUrl(url));
    requestInfo.setHeader(QNetworkRequest::ContentTypeHeader,QVariant(getFile_MEMI_Type(tmpLocal)));//服务器要求的数据头部

    // 发送数据
    QNetworkReply *reply =  m_pHttpMgr->post(requestInfo, &optFile);
    // 事件循环
    QEventLoop eventLoop;
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    //错误处理
    if (reply->error() == QNetworkReply::NoError)
    {
        qDebug() << "request protobufHttp NoError";
        m_optFile.retmsg = "上传成功！";
        m_optFile.retcode = "0";
    }
    else
    {
        //statusCodeV是HTTP服务器的相应码，reply->error()是Qt定义的错误码，可以参考QT的文档
        qDebug()<<"request protobufHttp handle errors here";
        QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qDebug( "request protobufHttp found error ....code: %d %d\n", statusCodeV.toInt(), (int)reply->error());
        qDebug(qPrintable(reply->errorString()));
        m_optFile.retmsg = "上传失败！";
        m_optFile.retmsg += reply->errorString();
        m_optFile.retcode = statusCodeV.toString();
    }
    //请求收到的结果
    //QByteArray responseByte = reply->readAll();
    //qDebug() << responseByte;
}

void GspUploadFile::postFile02(QString prjName, QString postUrl, QString fileName)
{
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

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(getFile_MEMI_Type(tmpLocal)));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"attachment \"; filename=\"" + optFile.fileName() + "\""));
    filePart.setBodyDevice(&optFile);
    optFile.setParent(multiPart);
    multiPart->append(filePart);

    QNetworkAccessManager* pNetManager = new QNetworkAccessManager;
    QUrl url(postUrl);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("image/jpeg"));
    QNetworkReply* reply = pNetManager->post(request, &optFile);

    QEventLoop eventLoop;
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(slot_uploadError(QNetworkReply::NetworkError)));
    QObject::connect(reply, SIGNAL(finished()),
                     this, SLOT(slot_uploadFinished()));
    //获取http状态码
    int http_code_ = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    // 获取数据
    if (QNetworkReply::NoError == reply->error())
    {
        QByteArray context_ = reply->readAll();
        QJsonObject result_obj = QJsonDocument::fromJson(context_).object();
        int code = result_obj.value("code").toInt();
        qDebug()<<"code === "<<code<<result_obj;
    }
    multiPart->setParent(reply);
    optFile.close();
    pNetManager->deleteLater();
    reply->deleteLater();
}

void GspUploadFile::slot_uploadError(QNetworkReply::NetworkError error)
{
    m_optFile.retcode = QString::number(error);
    m_optFile.retmsg = "上传错误！";
}

void GspUploadFile::slot_uploadFinished()
{
    m_optFile.retmsg = "上传成功！";
    m_optFile.retcode = "0";
}

void GspUploadFile::UpLoadForm(QString Path,QMap<QString,QString> params,QString fileFormName,QFile *uploadFile,QString newFileName){
   QString BOUNDARY = QUuid::createUuid().toString();
   QByteArray sb = QByteArray();

   //先上传普通的表单数据
   for(QMap<QString,QString>::Iterator t=params.begin();t!=params.end();t++){
       sb.append("--"+BOUNDARY+"\r\n");
       sb.append(QString("Content-Disposition: form-data; name=\"")+t.key()+QString("\"")+QString("\r\n"));
       sb.append("\r\n");
       sb.append(t.value()+"\r\n");
   }
   //上传文件的头部
   sb.append("--"+BOUNDARY+"\r\n");
   sb.append(QString("Content-Disposition: form-data; _type=\"")+fileFormName+QString("\"; fileName=\"")+newFileName+QString("\"")+QString("\r\n"));
   sb.append("\r\n");
   //上传文件内容
   if(!uploadFile->open(QIODevice::ReadOnly)){
       return;
   }
   sb.append(uploadFile->readAll());
   sb.append("\r\n");
   sb.append("--"+BOUNDARY+"\r\n");
   //编辑HTTP头部
   QNetworkAccessManager *_uploadManager=new QNetworkAccessManager();
   QNetworkRequest request = QNetworkRequest(QUrl(Path));
   request.setRawHeader(QString("Content-Type").toLatin1(),QString("multipart/form-data;boundary="+BOUNDARY).toLatin1());
   request.setRawHeader(QString("Content-Length").toLatin1(),QString::number(sb.length()).toLatin1());

   //执行post请求
    QNetworkReply* reply = _uploadManager->post(request,sb);

    QEventLoop eventLoop;
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
}

void GspUploadFile::postFile01(QString prjName, QString postUrl, QString fileName)
{
    QString tmpLocal = RW_ConfigFile::getInstence()->getRepositoryPath() + prjName + "\\" + fileName;
    QFile optFile(tmpLocal);
    if (!optFile.exists()) {//文件不存在
        m_optFile.retcode = "-1";
        m_optFile.retmsg = "文件不存在！";
        return ;
    }
    //if (!optFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    //    m_optFile.retcode = "-2";
    //    m_optFile.retmsg = "文件无法打开！";
    //    return ;
    //}
    QMap<QString,QString> params_send;
    params_send.insert("username","4556");
    QString fileFormName="file"; //上传文件表单中的名字
    QString newFileName="11.txt";
    UpLoadForm(postUrl, params_send, fileFormName, &optFile, newFileName);
}
