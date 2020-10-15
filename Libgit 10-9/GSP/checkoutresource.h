#ifndef CHECKOUTRESOURCE_H
#define CHECKOUTRESOURCE_H
#include "./qtwebserver/http/httpwebengine.h"

#include "gitcmdhandle.h"
#include "gitapihandle.h"
#include "jsondataparse.h"
#include "gspcheckout.h"

using namespace QtWebServer;

class CheckoutResource : public Http::Resource
{
public:
    CheckoutResource();
    ~CheckoutResource();

    void deliver(const Http::Request& request, Http::Response& response);
};

#endif // CHECKOUTRESOURCE_H
