#ifndef HTTPDAEMON_H
#define HTTPDAEMON_H
#include "./qtwebserver/tcp/tcpmultithreadedserver.h"
#include "./qtwebserver/http/httpwebengine.h"
#include "./qtwebserver/http/httpresource.h"

#include "qtservice.h"
#include "rootresource.h"
#include "cloneresource.h"
#include "checkoutresource.h"
#include "commitresource.h"
#include "logresource.h"
#include "statusresource.h"
#include "pullresource.h"
#include "rw_configfile.h"
#include "uploadfileresource.h"

using namespace QtWebServer;

class HttpDaemon : public Tcp::MultithreadedServer
{
public:
    HttpDaemon(quint16 port, QObject* parent = 0);
    bool startDaemon();
    void pause();
    void resume();

private:
    bool m_bDisabled; //暂无使能作用
    quint16 m_iport;
    Http::WebEngine* m_webEngine;
};

#endif // HTTPDAEMON_H
