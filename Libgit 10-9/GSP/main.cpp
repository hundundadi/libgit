#include <QCoreApplication>
#include <QSettings>
#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QDir>
#include "httpservice.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁
    static QMutex mutex;
    mutex.lock();

    //QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg("");
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal:");
        break;
    }

    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString strMessage = QString("File:%1  Line:%2  Function:%3  DateTime:%4  Message:%5")
            .arg(context.file).arg(context.line).arg(context.function).arg(strDateTime).arg(msg.constData());

    QString logPath = QString("%1\\%2")
            .arg(QCoreApplication::applicationDirPath())
            .arg(QDateTime::currentDateTime().toString("MM"));
    QDir dir(logPath);
    if (!dir.exists()) {
        dir.mkpath(logPath);
    }
    // 输出信息至文件中（读写、追加形式）
    QFile file(logPath + QString("\\%1_log.txt").arg(QDateTime::currentDateTime().toString("dd")));
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();

    // 解锁
    mutex.unlock();
}

int main(int argc, char **argv)
{
#if 1 // 服务启动用
    qInstallMessageHandler(myMessageOutput);
#if !defined(Q_OS_WIN)
    // QtService stores service settings in SystemScope, which normally require root privileges.
    // To allow testing this example as non-root, we change the directory of the SystemScope settings file.
    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QDir::tempPath());
    qWarning("(Example uses dummy settings file: %s/QtSoftware.conf)", QDir::tempPath().toLatin1().constData());
#endif
    HttpService service(argc, argv);
    return service.exec();
#else // 调试用
    QCoreApplication app(argc, argv);
    RW_ConfigFile::getInstence()->reReadRepositoryPath();
    quint16 port = RW_ConfigFile::getInstence()->getServerPort().toInt();
    HttpDaemon *m_daemon = new HttpDaemon(port, &app);
    qDebug() << "listen ing..." << port;
    if (!m_daemon->startDaemon())
    {
        qDebug() << "Failed to bind to port:" << port;
        app.quit();
    }
    return app.exec();
#endif
}
