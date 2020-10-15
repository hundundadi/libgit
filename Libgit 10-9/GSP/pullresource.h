#ifndef PULLRESOURCE_H
#define PULLRESOURCE_H
#include "./qtwebserver/http/httpwebengine.h"
#include "jsondataparse.h"
#include "gitcmdhandle.h"
#include "gitapihandle.h"
#include "gsppull.h"

using namespace QtWebServer;

class PullResource : public Http::Resource
{
public:
    PullResource();
    ~PullResource();
    void deliver(const Http::Request& request, Http::Response& response);
};

#endif // PULLRESOURCE_H
