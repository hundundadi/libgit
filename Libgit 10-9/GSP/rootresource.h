#ifndef RootResource_H
#define RootResource_H
#include "./qtwebserver/http/httpwebengine.h"

using namespace QtWebServer;

// Usually, this should go into its own files, of course.
class RootResource : public Http::Resource
{
public:
    RootResource();
    ~RootResource();

    void deliver(const Http::Request& request, Http::Response& response);
};

#endif // RootResource_H
