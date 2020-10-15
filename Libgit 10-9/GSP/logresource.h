#ifndef LOGRESOURCE_H
#define LOGRESOURCE_H
#include "./qtwebserver/http/httpwebengine.h"

#include "gitcmdhandle.h"
#include "gitapihandle.h"
#include "jsondataparse.h"
#include "gsplog.h"

using namespace QtWebServer;

class LogResource : public Http::Resource
{
public:
    LogResource();
    ~LogResource();

    void deliver(const Http::Request& request, Http::Response& response);
};

#endif // LOGRESOURCE_H
