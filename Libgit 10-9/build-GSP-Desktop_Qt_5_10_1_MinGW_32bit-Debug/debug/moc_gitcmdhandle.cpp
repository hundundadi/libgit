/****************************************************************************
** Meta object code from reading C++ file 'gitcmdhandle.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../GSP/gitcmdhandle.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gitcmdhandle.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GitCmdHandle_t {
    QByteArrayData data[15];
    char stringdata0[213];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GitCmdHandle_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GitCmdHandle_t qt_meta_stringdata_GitCmdHandle = {
    {
QT_MOC_LITERAL(0, 0, 12), // "GitCmdHandle"
QT_MOC_LITERAL(1, 13, 17), // "slot_statusResult"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 10), // "FileStatus"
QT_MOC_LITERAL(4, 43, 14), // "fileStatusList"
QT_MOC_LITERAL(5, 58, 21), // "slot_commitPushResult"
QT_MOC_LITERAL(6, 80, 13), // "ExecuteResult"
QT_MOC_LITERAL(7, 94, 8), // "stResult"
QT_MOC_LITERAL(8, 103, 16), // "slot_cloneResult"
QT_MOC_LITERAL(9, 120, 19), // "slot_checkoutResult"
QT_MOC_LITERAL(10, 140, 14), // "slot_logResult"
QT_MOC_LITERAL(11, 155, 16), // "QList<CommitLog>"
QT_MOC_LITERAL(12, 172, 13), // "listCommitlog"
QT_MOC_LITERAL(13, 186, 10), // "slot_Error"
QT_MOC_LITERAL(14, 197, 15) // "slot_pullResult"

    },
    "GitCmdHandle\0slot_statusResult\0\0"
    "FileStatus\0fileStatusList\0"
    "slot_commitPushResult\0ExecuteResult\0"
    "stResult\0slot_cloneResult\0slot_checkoutResult\0"
    "slot_logResult\0QList<CommitLog>\0"
    "listCommitlog\0slot_Error\0slot_pullResult"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GitCmdHandle[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x0a /* Public */,
       5,    1,   52,    2, 0x0a /* Public */,
       8,    1,   55,    2, 0x0a /* Public */,
       9,    1,   58,    2, 0x0a /* Public */,
      10,    1,   61,    2, 0x0a /* Public */,
      13,    1,   64,    2, 0x0a /* Public */,
      14,    1,   67,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,

       0        // eod
};

void GitCmdHandle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GitCmdHandle *_t = static_cast<GitCmdHandle *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slot_statusResult((*reinterpret_cast< FileStatus(*)>(_a[1]))); break;
        case 1: _t->slot_commitPushResult((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        case 2: _t->slot_cloneResult((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        case 3: _t->slot_checkoutResult((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        case 4: _t->slot_logResult((*reinterpret_cast< QList<CommitLog>(*)>(_a[1]))); break;
        case 5: _t->slot_Error((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        case 6: _t->slot_pullResult((*reinterpret_cast< ExecuteResult(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GitCmdHandle::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GitCmdHandle.data,
      qt_meta_data_GitCmdHandle,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *GitCmdHandle::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GitCmdHandle::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GitCmdHandle.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GitCmdHandle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
