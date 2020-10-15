#ifndef GSPCOMMIT_H
#define GSPCOMMIT_H
#include "gspbasecommon.h"
#include "gitapihandle.h"
#include "gsppull.h"

extern QString g_userName;
extern QString g_password;

class GspCommit : public GspBaseCommon
{
public:
    GspCommit();
    virtual ~GspCommit();

    void CommitFileToRemote(QString prjName, QString username, QString password, QString commitEmail, QString commitContent, QStringList fileList);
protected:
    int lg2_add(git_repository *repo, int argc, char **argv);
    int lg2_commit(git_repository *repo, int argc, char **argv);
    int lg2_push(git_repository *repo, int argc, char **argv, QStringList fileList);

    git_oid m_commit_oid;
private:
    QString m_commitLog;
    QString m_prjName;
    QString m_userInfo;
    QString m_userEmail;
};

#endif // GSPCOMMIT_H
