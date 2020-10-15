QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

QT += network

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    httpservice.cpp \
    httpdaemon.cpp \
    rw_configfile.cpp \
    gitcmdhandle.cpp \
    gitapihandle.cpp \
    jsondataparse.cpp \
    rootresource.cpp \
    cloneresource.cpp \
    checkoutresource.cpp \
    logresource.cpp \
    commitresource.cpp \
    statusresource.cpp \
    gspbasecommon.cpp \
    gspclone.cpp \
    gspcheckout.cpp \
    gspcommit.cpp \
    gspstatus.cpp \
    gsplog.cpp \
    gsppull.cpp \
    pullresource.cpp \
    gsptag.cpp \
    uploadfileresource.cpp \
    gspuploadfile.cpp

include(./windowservice/qtservice.pri)
LIBS += -L"D:\workspace\sunsheen\libgit\Libgit 10-9\GSP" -lgit2

INCLUDEPATH += ./qtwebserver
LIBS += -L"D:\workspace\sunsheen\libgit\Libgit 10-9\GSP\staticlib" -llibqtwebserver

HEADERS += \
    httpservice.h \
    httpdaemon.h \
    rw_configfile.h \
    gitcmdhandle.h \
    define.h \
    gitapihandle.h \
    jsondataparse.h \
    rootresource.h \
    cloneresource.h \
    checkoutresource.h \
    logresource.h \
    commitresource.h \
    statusresource.h \
    gspbasecommon.h \
    gspclone.h \
    gspcheckout.h \
    gspcommit.h \
    gspstatus.h \
    gsplog.h \
    gsppull.h \
    pullresource.h \
    gsptag.h \
    uploadfileresource.h \
    gspuploadfile.h
