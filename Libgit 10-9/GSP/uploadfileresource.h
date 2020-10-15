#ifndef UPLOADFILERESOURCE_H
#define UPLOADFILERESOURCE_H

#include <QFile>
#include <QTextCodec>

#include "./qtwebserver/http/httpwebengine.h"
#include "gitapihandle.h"
#include "jsondataparse.h"
#include "rw_configfile.h"
#include "gspuploadfile.h"

using namespace QtWebServer;

class UpLoadFileResource : public Http::Resource
{
public:
    UpLoadFileResource();
    ~UpLoadFileResource();

    void deliver(const Http::Request& request, Http::Response& response);
private:
    void ReadFileContent(QString prjName, QString fileName);
    void ResponseWeb(QString &ContentData, Http::Response &response);

    FileData m_optFile;
};

#endif // UPLOADFILERESOURCE_H
