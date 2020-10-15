#ifndef RW_CONFIGFILE_H
#define RW_CONFIGFILE_H

#include <QSettings>
#include <QString>
#include <QDebug>
#include <QCoreApplication>
#include <QTextCodec>
class RW_ConfigFile
{
public:
    static RW_ConfigFile* getInstence();
    void reReadRepositoryPath();
    QString getRepositoryPath();
    QString getServerPort();
protected:
    RW_ConfigFile();
    static RW_ConfigFile* m_pInstence;
private:
    bool ReadInitFile();
    bool WriteInitFile();
    QString m_strCurrentRepositoryPath; // 存储库路径
    QString m_strServerPort; // 服务监听端口
    QSettings* m_pSetting;
};

#endif // RW_CONFIGFILE_H
