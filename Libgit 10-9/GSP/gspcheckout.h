#ifndef GSPCHECKOUT_H
#define GSPCHECKOUT_H
#include "gspbasecommon.h"

class GspCheckout : public GspBaseCommon
{
public:
    GspCheckout();
    virtual ~GspCheckout();

    int lg2_checkout(git_repository *repo, int argc, char **argv);
    void CheckoutVersion(QString userName, QString password, QString prjName, QString checkoutVersion, QStringList fileList);
};

#endif // GSPCHECKOUT_H
