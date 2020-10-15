#include "rw_configfile.h"

RW_ConfigFile::RW_ConfigFile()
{
}

RW_ConfigFile* RW_ConfigFile::m_pInstence = nullptr;

RW_ConfigFile* RW_ConfigFile::getInstence()
{
    if (m_pInstence == nullptr)
    {
        m_pInstence = new RW_ConfigFile();
    }
    return m_pInstence;
}

// 重新读取存储库路径
void RW_ConfigFile::reReadRepositoryPath()
{
    if (!ReadInitFile())
    { // 如果读取失败，生成一个模板
        WriteInitFile();
    }
}

// 读取本地git存储仓库 的路径 --> 最后一个路径有效
bool RW_ConfigFile::ReadInitFile()
{
    m_pSetting = new QSettings(QCoreApplication::applicationDirPath() + "\\config.ini", QSettings::IniFormat);
    m_pSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));
    QString tmp;
    if(m_pSetting->allKeys().count() == 0)
        return false;
    foreach (QVariant var, m_pSetting->allKeys())
    {
        tmp = var.toString();
        if (tmp == "RepPath/Path")
            m_strCurrentRepositoryPath = m_pSetting->value(tmp).toString();
        if(tmp == "Server/Port")
            m_strServerPort = m_pSetting->value(tmp).toString();
    }// 取配置的路径 和 端口
    delete m_pSetting;
    return true;
}

// 为配置文件制作模板
bool RW_ConfigFile::WriteInitFile()
{
    m_pSetting = new QSettings(QCoreApplication::applicationDirPath() + "\\config.ini", QSettings::IniFormat);
    m_pSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));
    m_pSetting->setValue("/RepPath/Path", "c:\\xxx\\xxx\\xxx\\");
    m_pSetting->setValue("/Server/Port", "8080");
    delete m_pSetting;
    return true;
}

// 返回当前存储库路径
QString RW_ConfigFile::getRepositoryPath()
{
    return m_strCurrentRepositoryPath;
}

// 返回当前配置的端口
QString RW_ConfigFile::getServerPort()
{
    return m_strServerPort;
}
