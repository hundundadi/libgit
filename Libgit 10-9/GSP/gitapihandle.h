#ifndef GitApiHandle_H
#define GitApiHandle_H

#include <QString>
#include <QDebug>
#include <QObject>
#include <QFileInfo>
#include <QDir>
#include <QUuid>

#include "git2.h"

#include "rw_configfile.h"
#include "gitcmdhandle.h"
#include "define.h"
#include "gsplog.h"
#include "gspcommit.h"

extern CommitLog g_commitLog;

class GitApiHandle : public QObject
{
    Q_OBJECT
public:
    static GitApiHandle* getInstence();

    // 获取存储库文件状态
    bool gitStatus(QString projectName, QString username, QString password);
    int checkFileStatus(QString projectName, QString username, QString password);
    // 提交
    bool gitCommit(QString prjName, QString username, QString password, QString commitEmail, QString commitContent, QStringList fileList);

    // 克隆
    bool gitClone(QString userName, QString password, QString remote, QString prjName);
    // 版本回退
    bool gitCheckout(QString userName, QString password, QString prjName, QString checkoutVersion, QStringList fileList);
    // 日志查看
    bool gitLog(QString prjName, QString username, QString password, QStringList fileList);
    // 远程拉取
    bool gitPull(QString prjName, QString username, QString password, QString commitEmail, QStringList fileList, bool isWeb = false);
    bool gitPull_1(QString prjName, QString username, QString password, QString commitEmail, QStringList fileList, bool isWeb = false);
    bool gitPull_2(QString prjName, QString username, QString password, QString commitEmail, QStringList fileList, bool isWeb = false);
public:
    bool updateWebPage();
    QString caculateSuitFileSizeUnit(qint64 size);
    QList<FileStatus> m_FileList;
private:
    explicit GitApiHandle(QObject *parent = nullptr);
    ~GitApiHandle();
    static GitApiHandle* m_pInstence;
    // 获取存储库文件状态
signals:
    void signal_FileStatus(FileStatus fileStatusList);
    void signal_CommitResult(ExecuteResult stResult);
    void signal_CloneResult(ExecuteResult stResult);
    void signal_CheckoutResult(ExecuteResult stResult);
    void signal_LogResult(QList<CommitLog> listCommitlog);

    void signal_Error(ExecuteResult stResult);
protected:
    int git2_InitAndOpen();

    // 提交
    int git2_GetHead(git_commit **pParentCommit);
    int git2_AddFile(git_index **pIndex, QStringList fileList);
    int git2_AddToTree(git_tree **pNewTree, QStringList fileList);
    int git2_PushRemote();
    ExecuteResult git2_ExecuteError(int error);
    QStringList m_confFileList;
public:
    QString m_sUserName;
    QString m_sPassword;
    QString m_sPrjName;
    QString m_sLocalRep;

    // 提交时候的合并记录
    QString m_scommitLog;
private:
    QString fileStatus(unsigned int status_flags);
    void OptionsRepository();

    // status
    FileStatus getRepostoryFileAndDir(QString path, FileStatus &pramfile, int &iGrade);
    bool updateFileStatus(QStringList repFileList, FileStatus &fileList);
    bool updateFileStatus_1(QStringList repFileList, FileStatus &fileList);
    void AddRepoDeletedDir(QStringList repFileList, FileStatus &fileList);
    void AddRepoDeletedFile(QStringList repFileList, FileStatus &fileList, int &iGrade);
    void AddRepoDeletedFile_1(QStringList repFileList, FileStatus &fileList, int &iGrade);
    bool catchSignalFileLogVersion(QString fileRelativePath);
    QString m_strCurrentStatus;
    QString m_strCurrentVersion;
    bool m_bRepDeletedFile;
    bool m_bRepSamePath;
    QString m_strRelativePath; // 已删除文件夹的文件相对路径
    QString m_strDir_QUUID; // 距离删除文件夹最近的上级文件夹的QUUID

    // log
    int match_with_parent(git_commit *commit, int i, git_diff_options *opts);
    void print_commit(git_commit *commit);
    QString print_time(const git_time *intime);
    QList<CommitLog> m_listCommitlog;
    // Git存储仓库句柄
    git_repository* m_pGitRepository;

    size_t m_iGitParentCount;
    const git_commit* m_pParents[1];
};

#endif // GitApiHandle_H
