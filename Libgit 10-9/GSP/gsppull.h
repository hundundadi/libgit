#ifndef GSPPULL_H
#define GSPPULL_H

#include "gspbasecommon.h"

class GspPull : public GspBaseCommon
{
public:
    GspPull();
    virtual ~GspPull();

    ExecuteResult m_stResult;
    void Pull(QString prjName, QString username, QString password, QString commitEmail, QString remoteUrl, bool isRequest = false);
private:
    int lg2_merge(git_repository *repo, int argc, char **argv);
    void output_conflicts(git_index *index);
    int create_merge_commit(git_repository *repo, git_index *index, struct merge_options *opts);

    int lg2_fetch(git_repository *repo, int argc, char **argv);
    QString m_commitEmail;
};

#endif // GSPPULL_H
