#ifndef GSPCLONE_H
#define GSPCLONE_H
#include "gspbasecommon.h"

extern QString g_userName;
extern QString g_password;

class GspClone : public GspBaseCommon
{
public:
    GspClone();
    virtual ~GspClone();

    void Clone(QString userName, QString password, QString remote, QString prjName);
    int lg2_clone(int argc, char **argv);
};

#endif // GSPCLONE_H
