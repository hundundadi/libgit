#ifndef JSONDATAPARSE_H
#define JSONDATAPARSE_H
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

#include "define.h"

class JsonDataParse
{
public:
    static JsonDataParse* getInstence();

    bool startParseJson(QString &jsonContent);
    QString getValue(QString mapKey);
    QStringList getFileList();
    QString generateFileStatusJsonString(FileStatus fileList);
    QString generateCommitLogJsonString(QList<CommitLog> loglist);
    QString generateNormalResultJsonString(ExecuteResult stResult);
    QString generateUploadFileContentJsonString(FileData fileData);
protected:
    BodyJsonData m_stJsonData;
private:
    JsonDataParse();
    static JsonDataParse* m_pInstence;

    // 清空数据
    void clearStJsonData();
};

#endif // JSONDATAPARSE_H
