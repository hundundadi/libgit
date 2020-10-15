#ifndef GITCOMMAND_H
#define GITCOMMAND_H
#include <QString>
#include <QStringList>
#include <QMap>

#define GIT_PULL "git pull"
#define GIT_ADD "git add"
#define GIT_COMMIT "git commit -m"
#define GIT_PUSH "git push"
#define GIT_CLONE "git clone"
#define GIT_CHECKOUT "git checkout"
#define GIT_CONFIGNAME "git config --global user.name"
#define GIT_CONFIGEMAIL "git config --global user.email"

#define byteToGB (1024 * 1024 * 1024)
#define byteToMB (1024 * 1024)
#define byteToKB 1024

typedef struct RealFileStatus
{
    QString deptid; // uuid
    QString grade; // 等级
    QString belongto; // 归属
    QString file; // 文件名
    QString status; // 状态
    QString user; // 拥有者
    QString date; // 最后修改日期
    QString relativepath; // 文件相对路径
    QString version; // 版本
    QString size; // 大小
    QList<RealFileStatus> children; // 子文件
}FileStatus;

typedef struct
{
    QStringList conflictFileList;
    QString statusCode;
    QString content;
}ExecuteResult;

typedef struct
{
    //QString _ProjectName; // 工程名
    //QString _UserName; // 用户名
    //QString _Password; // 密码
    //QString _CommitContent; // 提交说明内容
    //QString _CloneToLocal; // 克隆的Pc机位置 -- 废弃=改用本地配置+工程名
    //QString _CloneFromRemote; // 克隆的远端地址
    //QString _CheckoutVersion; // 回退版本
    //QString _CommitEmail; // 提交邮箱
    //QString _FilePostUrl; // 文件上传地址
    QStringList _FileList; // 文件列表 文件 _File
    QMap<QString, QString> _mapJsonData; // 键值存储
}BodyJsonData;

typedef struct
{
    QString author; // 提交者
    QString email; // 邮箱
    QString dateTime; // 日期
    QString commit_sha; // 版本sha
    QString commit_info; // 提交信息
    QStringList mergeFrom; // 合并自不同的父sha
}CommitLog;

typedef struct
{
    QString file; // 文件
    QString size; // 大小
    QString content; // 内容
    QString retcode; // 状态码
    QString retmsg; // 操作结果
}FileData;

#endif // GITCOMMAND_H
