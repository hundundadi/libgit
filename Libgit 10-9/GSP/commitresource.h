#ifndef COMMITRESOURCE_H
#define COMMITRESOURCE_H
#include "./qtwebserver/http/httpwebengine.h"

#include "gitcmdhandle.h"
#include "gitapihandle.h"
#include "jsondataparse.h"
#include "gspcommit.h"

using namespace QtWebServer;

class CommitResource : public Http::Resource
{
public:
    CommitResource();
    ~CommitResource();

    void deliver(const Http::Request& request, Http::Response& response);
};

#endif // COMMITRESOURCE_H
