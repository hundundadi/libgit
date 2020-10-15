#ifndef GSPTAG_H
#define GSPTAG_H

#include "gspbasecommon.h"

extern QString g_userName;
extern QString g_password;

class GspTag : public GspBaseCommon
{
public:
    GspTag();
    ~GspTag();

    void Tag(QString prjName, QString username, QString password, QString commitEmail, QString opt);
private:
    int lg2_tag(git_repository *repo, int argc, char **argv);

    QString m_userEmail;
    ExecuteResult m_stResult;
};

#endif // GSPTAG_H
