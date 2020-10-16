/****************************************************************************
** Meta object code from reading C++ file 'gspbasecommon.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../GSP/gspbasecommon.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gspbasecommon.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GspBaseCommon_t {
    QByteArrayData data[15];
    char stringdata0[222];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GspBaseCommon_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GspBaseCommon_t qt_meta_stringdata_GspBaseCommon = {
    {
QT_MOC_LITERAL(0, 0, 13), // "GspBaseCommon"
QT_MOC_LITERAL(1, 14, 17), // "signal_FileStatus"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 10), // "FileStatus"
QT_MOC_LITERAL(4, 44, 14), // "fileStatusList"
QT_MOC_LITERAL(5, 59, 19), // "signal_CommitResult"
QT_MOC_LITERAL(6, 79, 13), // "ExecuteResult"
QT_MOC_LITERAL(7, 93, 8), // "stResult"
QT_MOC_LITERAL(8, 102, 18), // "signal_CloneResult"
QT_MOC_LITERAL(9, 121, 21), // "signal_CheckoutResult"
QT_MOC_LITERAL(10, 143, 16), // "signal_LogResult"
QT_MOC_LITERAL(11, 160, 16), // "QList<CommitLog>"
QT_MOC_LITERAL(12, 177, 13), // "listCommitlog"
QT_MOC_LITERAL(13, 191, 17), // "signal_PullResult"
QT_MOC_LITERAL(14, 209, 12) // "signal_Error"

    },
    "GspBaseCommon\0signal_FileStatus\0\0"
    "FileStatus\0fileStatusList\0signal_CommitResult\0"
    "ExecuteResult\0stResult\0signal_CloneResult\0"
    "signal_CheckoutResult\0signal_LogResult\0"
    "QList<CommitLog>\0listCommitlog\0"
    "signal_PullResult\0signal_Error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GspBaseCommon[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       5,    1,   52,    2, 0x06 /* Public */,
       8,    1,   55,    2, 0x06 /* Public */,
       9,    1,   58,    2, 0x06 /* Public */,
      10,    1,   61,    2, 0x06 /* Public */,
      13,    1,   64,    2, 0x06 /* Public */,
      14,    1,   67,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,

       0        // eod
};

void GspBaseCommon::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GspBaseCommon *_t = static_cast<GspBaseCommon *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signal_FileStatus((*reinterpret_cast< FileStatus(*)>(_a[1]))); break;
        case 1: _t->signal_CommitResult((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        case 2: _t->signal_CloneResult((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        case 3: _t->signal_CheckoutResult((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        case 4: _t->signal_LogResult((*reinterpret_cast< QList<CommitLog>(*)>(_a[1]))); break;
        case 5: _t->signal_PullResult((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        case 6: _t->signal_Error((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (GspBaseCommon::*_t)(FileStatus );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GspBaseCommon::signal_FileStatus)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (GspBaseCommon::*_t)(ExecuteResult );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GspBaseCommon::signal_CommitResult)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (GspBaseCommon::*_t)(ExecuteResult );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GspBaseCommon::signal_CloneResult)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (GspBaseCommon::*_t)(ExecuteResult );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GspBaseCommon::signal_CheckoutResult)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (GspBaseCommon::*_t)(QList<CommitLog> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GspBaseCommon::signal_LogResult)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (GspBaseCommon::*_t)(ExecuteResult );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GspBaseCommon::signal_PullResult)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (GspBaseCommon::*_t)(ExecuteResult );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GspBaseCommon::signal_Error)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GspBaseCommon::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GspBaseCommon.data,
      qt_meta_data_GspBaseCommon,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *GspBaseCommon::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GspBaseCommon::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GspBaseCommon.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GspBaseCommon::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void GspBaseCommon::signal_FileStatus(FileStatus _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GspBaseCommon::signal_CommitResult(ExecuteResult _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GspBaseCommon::signal_CloneResult(ExecuteResult _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GspBaseCommon::signal_CheckoutResult(ExecuteResult _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void GspBaseCommon::signal_LogResult(QList<CommitLog> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void GspBaseCommon::signal_PullResult(ExecuteResult _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void GspBaseCommon::signal_Error(ExecuteResult _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
