/****************************************************************************
** Meta object code from reading C++ file 'gitapihandle.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../GSP/gitapihandle.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gitapihandle.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GitApiHandle_t {
    QByteArrayData data[14];
    char stringdata0[203];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GitApiHandle_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GitApiHandle_t qt_meta_stringdata_GitApiHandle = {
    {
QT_MOC_LITERAL(0, 0, 12), // "GitApiHandle"
QT_MOC_LITERAL(1, 13, 17), // "signal_FileStatus"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 10), // "FileStatus"
QT_MOC_LITERAL(4, 43, 14), // "fileStatusList"
QT_MOC_LITERAL(5, 58, 19), // "signal_CommitResult"
QT_MOC_LITERAL(6, 78, 13), // "ExecuteResult"
QT_MOC_LITERAL(7, 92, 8), // "stResult"
QT_MOC_LITERAL(8, 101, 18), // "signal_CloneResult"
QT_MOC_LITERAL(9, 120, 21), // "signal_CheckoutResult"
QT_MOC_LITERAL(10, 142, 16), // "signal_LogResult"
QT_MOC_LITERAL(11, 159, 16), // "QList<CommitLog>"
QT_MOC_LITERAL(12, 176, 13), // "listCommitlog"
QT_MOC_LITERAL(13, 190, 12) // "signal_Error"

    },
    "GitApiHandle\0signal_FileStatus\0\0"
    "FileStatus\0fileStatusList\0signal_CommitResult\0"
    "ExecuteResult\0stResult\0signal_CloneResult\0"
    "signal_CheckoutResult\0signal_LogResult\0"
    "QList<CommitLog>\0listCommitlog\0"
    "signal_Error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GitApiHandle[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       5,    1,   47,    2, 0x06 /* Public */,
       8,    1,   50,    2, 0x06 /* Public */,
       9,    1,   53,    2, 0x06 /* Public */,
      10,    1,   56,    2, 0x06 /* Public */,
      13,    1,   59,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 6,    7,

       0        // eod
};

void GitApiHandle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GitApiHandle *_t = static_cast<GitApiHandle *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signal_FileStatus((*reinterpret_cast< FileStatus(*)>(_a[1]))); break;
        case 1: _t->signal_CommitResult((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        case 2: _t->signal_CloneResult((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        case 3: _t->signal_CheckoutResult((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        case 4: _t->signal_LogResult((*reinterpret_cast< QList<CommitLog>(*)>(_a[1]))); break;
        case 5: _t->signal_Error((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (GitApiHandle::*_t)(FileStatus );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GitApiHandle::signal_FileStatus)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (GitApiHandle::*_t)(ExecuteResult );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GitApiHandle::signal_CommitResult)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (GitApiHandle::*_t)(ExecuteResult );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GitApiHandle::signal_CloneResult)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (GitApiHandle::*_t)(ExecuteResult );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GitApiHandle::signal_CheckoutResult)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (GitApiHandle::*_t)(QList<CommitLog> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GitApiHandle::signal_LogResult)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (GitApiHandle::*_t)(ExecuteResult );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GitApiHandle::signal_Error)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GitApiHandle::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GitApiHandle.data,
      qt_meta_data_GitApiHandle,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *GitApiHandle::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GitApiHandle::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GitApiHandle.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GitApiHandle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void GitApiHandle::signal_FileStatus(FileStatus _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GitApiHandle::signal_CommitResult(ExecuteResult _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GitApiHandle::signal_CloneResult(ExecuteResult _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GitApiHandle::signal_CheckoutResult(ExecuteResult _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void GitApiHandle::signal_LogResult(QList<CommitLog> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void GitApiHandle::signal_Error(ExecuteResult _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
