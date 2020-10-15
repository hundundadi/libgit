#ifndef CloneResource_H
#define CloneResource_H
#include "./qtwebserver/http/httpwebengine.h"

#include "gitapihandle.h"
#include "jsondataparse.h"
#include "gspclone.h"

using namespace QtWebServer;

class CloneResource : public Http::Resource
{
public:
    CloneResource();
    ~CloneResource();

    void deliver(const Http::Request& request, Http::Response& response);

    void Test_Html_Page(Http::Response& response);

private:
};

#endif // CloneResource_H
