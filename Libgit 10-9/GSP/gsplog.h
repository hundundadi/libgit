#ifndef GSPLOG_H
#define GSPLOG_H

#include "gspbasecommon.h"

extern CommitLog g_commitLog;

class GspLog : public GspBaseCommon
{
public:
    GspLog();
    virtual ~GspLog();

    void Log(QString prjName, QString username, QString password, QStringList fileList);
private:
    int lg2_log(git_repository *repo, int argc, char *argv[]);
    /** utility functions that are called to configure the walker */
    void push_rev(struct log_state *s, git_object *obj, int hide);
    int add_revision(struct log_state *s, const char *revstr);
    void set_sorting(struct log_state *s, unsigned int sort_mode);
    /** utility functions that parse options and help with log output */
    int match_with_parent(git_commit *commit, int i, git_diff_options *opts);
    void print_time(const git_time *intime, const char *prefix);
    void print_commit(git_commit *commit, struct log_options *opts);
    int parse_options(struct log_state *s, struct log_options *opt, int argc, char **argv);
};

#endif // GSPLOG_H
