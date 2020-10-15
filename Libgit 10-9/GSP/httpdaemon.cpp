#include "httpdaemon.h"

HttpDaemon::HttpDaemon(quint16 port, QObject* parent)
    : MultithreadedServer(parent), m_bDisabled(false), m_iport(port)
{
    m_webEngine = new Http::WebEngine(parent);
}

bool HttpDaemon::startDaemon()
{
    // Set logging mode to output on console
    Log::instance()->setLoggingMode(Log::LoggingModeConsole);

    // Create a web engine

    m_webEngine->addResource(new RootResource()); // a html resource file
    m_webEngine->addResource(new CloneResource());
    m_webEngine->addResource(new CheckoutResource());
    m_webEngine->addResource(new CommitResource());
    m_webEngine->addResource(new LogResource());
    m_webEngine->addResource(new StatusResource());
    m_webEngine->addResource(new PullResource());
    m_webEngine->addResource(new UpLoadFileResource());

    // Create and launch a server that for our web engine
    //Tcp::MultithreadedServer tcpServer;
    this->setResponder(m_webEngine);
    this->listen(QHostAddress::Any, m_iport, 16);
    if(!this->isListening())
    {
        return false;
    }
    // 首次启动服务时，即刻加载配置文件，为后续操作做好准备
    RW_ConfigFile::getInstence()->reReadRepositoryPath();
    return true;
}

void HttpDaemon::pause()
{
    m_bDisabled = true;
}

void HttpDaemon::resume()
{
    m_bDisabled = false;
}
