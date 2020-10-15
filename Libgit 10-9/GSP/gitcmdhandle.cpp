#include "gitcmdhandle.h"

GitCmdHandle* GitCmdHandle::m_pInstence = nullptr;

GitCmdHandle::GitCmdHandle(QObject *parent) : QObject(parent)
{

}

GitCmdHandle* GitCmdHandle::getInstence()
{
    if (m_pInstence == nullptr)
    {
        m_pInstence = new GitCmdHandle();
    }
    return m_pInstence;
}

void GitCmdHandle::setResponse(Http::Response *res)
{
    m_pResponse = res;
}

/* {"data":[{
 * "filename":"aa.txt,
 * "status":文件状态,
 * "update_user":zdc",
 * "update_date":”2020-02-23 12:00”,
 * ver_desc":"版本描述",
 * file_size":10.00kb}],
 * "retmsg":"获取成功！",
 * "retcode":"1"} */
void GitCmdHandle::slot_statusResult(FileStatus fileStatusList)
{
    QString body = JsonDataParse::getInstence()->generateFileStatusJsonString(fileStatusList);
    resultToWeb(body);
    qDebug() << body;
}
/* {"retmsg":"提交成功！",
 * "retcode":"1"} */
void GitCmdHandle::slot_commitPushResult(ExecuteResult stResult)
{
    QString body = JsonDataParse::getInstence()->generateNormalResultJsonString(stResult);
    resultToWeb(body);
    qDebug() << body;
}
/* {retmsg:”已克隆”,”retcode”:”3”}
 * {"retmsg":"克隆成功！","retcode":"1"}
 * {"retmsg":"失败原因","retcode":"0"}
 */
void GitCmdHandle::slot_cloneResult(ExecuteResult stResult)
{
    QString body = JsonDataParse::getInstence()->generateNormalResultJsonString(stResult);
    resultToWeb(body);
    qDebug() << body;
}
/* {retmsg:”返回成功”,”retcode”:”1”, data[{"ver_num":"123",”ver_desc":版本描述," update_user ":zdc,"update_date":"更新时间" }]}
 * {"retmsg":"回退成功！","retcode":"1"，文件信息}
 * {"retmsg":"失败原因","retcode":"0"}
 */
void GitCmdHandle::slot_checkoutResult(ExecuteResult stResult)
{
    QString body = JsonDataParse::getInstence()->generateNormalResultJsonString(stResult);
    resultToWeb(body);
    qDebug() << body;
}
/* {retmsg:”返回成功”,”retcode”:”1”, data[{"ver_num":"123",”ver_desc":版本描述," update_user ":zdc,"update_date":"更新时间" }]}
 * {"retmsg":"失败原因","retcode":"0"}
 */
void GitCmdHandle::slot_logResult(QList<CommitLog> listCommitlog)
{
    QString body = JsonDataParse::getInstence()->generateCommitLogJsonString(listCommitlog);
    resultToWeb(body);
    qDebug() << body;
}

void GitCmdHandle::slot_pullResult(ExecuteResult stResult)
{
    QString body = JsonDataParse::getInstence()->generateNormalResultJsonString(stResult);
    resultToWeb(body);
    qDebug() << body;
}

void GitCmdHandle::slot_Error(ExecuteResult stResult)
{
    QString body = JsonDataParse::getInstence()->generateNormalResultJsonString(stResult);
    resultToWeb(body);
    qDebug() << body;
}

void GitCmdHandle::resultToWeb(QString result)
{
    m_pResponse->setStatusCode(Http::Ok);
    m_pResponse->setHeader(Http::ContentType, "application/json");
    m_pResponse->setHeader("Access-Control-Allow-Origin", "*");
    m_pResponse->setHeader(Http::ContentLength, "*");
    m_pResponse->setHeader(Http::Authorization, "*");
    m_pResponse->setHeader(Http::Accept, "*");
    m_pResponse->setHeader(Http::Allow, "*");
    m_pResponse->setBody(result.toUtf8());
}
