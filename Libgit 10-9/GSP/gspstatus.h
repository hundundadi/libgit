#ifndef GSPSTATUS_H
#define GSPSTATUS_H

#include "gspbasecommon.h"


class GspStatus : public GspBaseCommon
{
public:
    GspStatus();
    virtual ~GspStatus();

    void Status(QString projectName, QString username, QString password);
};

#endif // GSPSTATUS_H
