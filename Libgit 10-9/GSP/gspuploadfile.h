#ifndef GSPUPLOADFILE_H
#define GSPUPLOADFILE_H

#include <QObject>
#include <QFile>

#include <QMimeDatabase>
#include <QMimeType>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUuid>

#include <QJsonDocument>
#include <QJsonObject>

#include "define.h"
#include "rw_configfile.h"

class GspUploadFile : public QObject
{
    Q_OBJECT
public:
    explicit GspUploadFile(QObject *parent = nullptr);

    void postFile(QString prjName, QString postUrl, QString fileName);
    void postFile01(QString prjName, QString postUrl, QString fileName);
    void postFile02(QString prjName, QString postUrl, QString fileName);
    FileData m_optFile;
signals:

public slots:
    void slot_uploadError(QNetworkReply::NetworkError error);
    void slot_uploadFinished();
private:
    void UpLoadForm(QString Path,QMap<QString,QString> params,QString fileFormName,QFile *uploadFile,QString newFileName);
    QString getFile_MEMI_Type(const QString &filePath);
};

#endif // GSPUPLOADFILE_H
