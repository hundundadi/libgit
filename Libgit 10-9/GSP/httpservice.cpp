#include "httpservice.h"

HttpService::HttpService(int argc, char **argv)
: QtService<QCoreApplication>(argc, argv, "Git_Client_Server Daemon")
{
    setServiceDescription("用Qt实现的虚拟http服务。");
    setServiceFlags(QtServiceBase::CanBeSuspended);
}

void HttpService::start()
{
    QCoreApplication *app = application();
    RW_ConfigFile::getInstence()->reReadRepositoryPath();
    quint16 port = RW_ConfigFile::getInstence()->getServerPort().toInt();
    m_daemon = new HttpDaemon(port, app);
    qDebug() << "listen ing..." << port;
    if (!m_daemon->startDaemon())
    {
        logMessage(QString("Failed to bind to port %1").arg(port), QtServiceBase::Error);
        qDebug() << "Failed to bind to port:" << port;
        app->quit();
    }

}

void HttpService::pause()
{
    m_daemon->pause();
}

void HttpService::resume()
{
    m_daemon->resume();
}

