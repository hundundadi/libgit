#ifndef HTTPSERVICE_H
#define HTTPSERVICE_H
#include <QStringList>
#include "qtservice.h"
#include "httpdaemon.h"
#include "rw_configfile.h"

class HttpService : public QtService<QCoreApplication>
{
public:
    HttpService(int argc, char **argv);

protected:
    void start();
    void pause();
    void resume();

private:
    HttpDaemon* m_daemon;
};

#endif // HTTPSERVICE_H
