#ifndef GITCMDHANDLE_H
#define GITCMDHANDLE_H

#include <QObject>
#include <QProcess>
#include <QDebug>
#include "./qtwebserver/http/httpwebengine.h"
#include "define.h"
#include "jsondataparse.h"

using namespace QtWebServer;

class GitCmdHandle : public QObject
{
    Q_OBJECT
public:
    static GitCmdHandle* getInstence();

    void setResponse(Http::Response* res);
private:
    explicit GitCmdHandle(QObject *parent = nullptr);
    static GitCmdHandle* m_pInstence;
public slots:
    //void slot_commitNumResult(); // 历史提交版本结果返回
    void slot_statusResult(FileStatus fileStatusList); // 文件状态返回
    void slot_commitPushResult(ExecuteResult stResult); // 提交结果返回
    void slot_cloneResult(ExecuteResult stResult); // 克隆结果返回
    void slot_checkoutResult(ExecuteResult stResult); // 版本回退结果返回
    void slot_logResult(QList<CommitLog> listCommitlog); // 日志结果返回
    void slot_Error(ExecuteResult stResult); // 执行出错结果返回
    void slot_pullResult(ExecuteResult stResult); // 拉取的结果返回
private:
    Http::Response* m_pResponse;
    void resultToWeb(QString result);
};

#endif // GITCMDHANDLE_H
