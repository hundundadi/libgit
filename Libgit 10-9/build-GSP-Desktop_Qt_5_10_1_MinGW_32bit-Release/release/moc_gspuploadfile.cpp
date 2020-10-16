/****************************************************************************
** Meta object code from reading C++ file 'gspuploadfile.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../GSP/gspuploadfile.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gspuploadfile.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GspUploadFile_t {
    QByteArrayData data[6];
    char stringdata0[86];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GspUploadFile_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GspUploadFile_t qt_meta_stringdata_GspUploadFile = {
    {
QT_MOC_LITERAL(0, 0, 13), // "GspUploadFile"
QT_MOC_LITERAL(1, 14, 16), // "slot_uploadError"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 27), // "QNetworkReply::NetworkError"
QT_MOC_LITERAL(4, 60, 5), // "error"
QT_MOC_LITERAL(5, 66, 19) // "slot_uploadFinished"

    },
    "GspUploadFile\0slot_uploadError\0\0"
    "QNetworkReply::NetworkError\0error\0"
    "slot_uploadFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GspUploadFile[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x0a /* Public */,
       5,    0,   27,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

       0        // eod
};

void GspUploadFile::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GspUploadFile *_t = static_cast<GspUploadFile *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slot_uploadError((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        case 1: _t->slot_uploadFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply::NetworkError >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GspUploadFile::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GspUploadFile.data,
      qt_meta_data_GspUploadFile,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *GspUploadFile::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GspUploadFile::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GspUploadFile.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GspUploadFile::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
