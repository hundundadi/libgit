#ifndef GSPBASECOMMON_H
#define GSPBASECOMMON_H

#include <QObject>
#include <assert.h>

#include "git2.h"
#include "rw_configfile.h"
#include "define.h"
#include "gitcmdhandle.h"

#ifndef PRIuZ
/* Define the printf format specifer to use for size_t output */
#if defined(_MSC_VER) || defined(__MINGW32__)
#	define PRIuZ "Iu"
#else
#	define PRIuZ "zu"
#endif
#endif

#define UNUSED(x) (void)(x)

extern QString g_userName;
extern QString g_password;
extern CommitLog g_commitLog;
struct args_info {
    int    argc;
    char **argv;
    int    pos;
    int    opts_done : 1; /**< Did we see a -- separator */
};
#define ARGS_INFO_INIT { argc, argv, 0, 0 }
#define ARGS_CURRENT(args) args->argv[args->pos]
extern void *xrealloc(void *oldp, size_t newsz);
extern int diff_output(const git_diff_delta *d, const git_diff_hunk *h, const git_diff_line *l, void *p);
extern size_t is_prefixed(const char *str, const char *pfx);
extern int match_str_arg(const char **out, struct args_info *args, const char *opt);
extern int match_bool_arg(int *out, struct args_info *args, const char *opt);
extern int match_arg_separator(struct args_info *args);
extern void strarray_from_args(git_strarray *array, struct args_info *args);
extern int match_int_arg(int *out, struct args_info *args, const char *opt, int allow_negative);
extern int is_integer(int *out, const char *str, int allow_negative);
extern int cred_acquire_cb(git_credential **out,
        const char *url,
        const char *username_from_url,
        unsigned int allowed_types,
        void *payload);
extern int resolve_refish(git_annotated_commit **commit, git_repository *repo, const char *refish);
extern void fatal(const char *message, const char *extra);

class GspBaseCommon : public QObject
{
    Q_OBJECT
public:
    explicit GspBaseCommon(QObject *parent = nullptr);

signals:
    void signal_FileStatus(FileStatus fileStatusList);
    void signal_CommitResult(ExecuteResult stResult);
    void signal_CloneResult(ExecuteResult stResult);
    void signal_CheckoutResult(ExecuteResult stResult);
    void signal_LogResult(QList<CommitLog> listCommitlog);
    void signal_PullResult(ExecuteResult stResult);

    void signal_Error(ExecuteResult stResult);
public slots:

protected:
    void check_lg2(int error, const char *message, const char *extra);
    ExecuteResult git2_ExecuteError(int error);

    // 克隆 - git clone xxx
    virtual void Clone(QString userName, QString password, QString remote, QString prjName){}
    // 版本回退 -  git checkout xxx filelist
    virtual void CheckoutVersion(QString userName, QString password, QString prjName, QString checkoutVersion, QStringList fileList){}
    // 仓库文件提交 - git add + git commit -m + git push origin
    virtual void CommitFileToRemote(QString prjName, QString username, QString password, QString commitEmail, QString commitContent, QStringList fileList){}
    // 状态 - git status
    virtual void Status(QString projectName, QString username, QString password){}
    // 提交记录查询 - git log file
    virtual void Log(QString prjName, QString username, QString password, QStringList fileList){}
    // 拉取服务器数据 - git pull
    virtual void Pull(QString prjName, QString username, QString password, QString commitEmail, QString remoteUrl, bool isRequest = false){}
    // 标签操作（列出所有标签、创建标签、删除标签）
    virtual void Tag(QString prjName, QString username, QString password, QString commitEmail, QString opt){}
private:

};

#endif // GSPBASECOMMON_H
