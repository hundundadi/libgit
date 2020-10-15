#ifndef STATUSRESOURCE_H
#define STATUSRESOURCE_H
#include "./qtwebserver/http/httpwebengine.h"

#include "gitcmdhandle.h"
#include "gitapihandle.h"
#include "jsondataparse.h"

using namespace QtWebServer;

class StatusResource : public Http::Resource
{
public:
    StatusResource();
    ~StatusResource();

    void deliver(const Http::Request& request, Http::Response& response);
};

#endif // STATUSRESOURCE_H
