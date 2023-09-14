/****************************************************************************
** Meta object code from reading C++ file 'filedialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../xiaowang/filedialog/filedialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filedialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Save_FileDialog_t {
    QByteArrayData data[9];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Save_FileDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Save_FileDialog_t qt_meta_stringdata_Save_FileDialog = {
    {
QT_MOC_LITERAL(0, 0, 15), // "Save_FileDialog"
QT_MOC_LITERAL(1, 16, 17), // "directorySelected"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 4), // "path"
QT_MOC_LITERAL(4, 40, 15), // "selectDirectory"
QT_MOC_LITERAL(5, 56, 11), // "updateLabel"
QT_MOC_LITERAL(6, 68, 11), // "QModelIndex"
QT_MOC_LITERAL(7, 80, 5), // "index"
QT_MOC_LITERAL(8, 86, 14) // "saveTextToFile"

    },
    "Save_FileDialog\0directorySelected\0\0"
    "path\0selectDirectory\0updateLabel\0"
    "QModelIndex\0index\0saveTextToFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Save_FileDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   37,    2, 0x0a /* Public */,
       5,    1,   38,    2, 0x0a /* Public */,
       8,    1,   41,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void Save_FileDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Save_FileDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->directorySelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->selectDirectory(); break;
        case 2: _t->updateLabel((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 3: _t->saveTextToFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Save_FileDialog::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Save_FileDialog::directorySelected)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Save_FileDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<AppWindow::staticMetaObject>(),
    qt_meta_stringdata_Save_FileDialog.data,
    qt_meta_data_Save_FileDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Save_FileDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Save_FileDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Save_FileDialog.stringdata0))
        return static_cast<void*>(this);
    return AppWindow::qt_metacast(_clname);
}

int Save_FileDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AppWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Save_FileDialog::directorySelected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_Load_FileDialog_t {
    QByteArrayData data[9];
    char stringdata0[93];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Load_FileDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Load_FileDialog_t qt_meta_stringdata_Load_FileDialog = {
    {
QT_MOC_LITERAL(0, 0, 15), // "Load_FileDialog"
QT_MOC_LITERAL(1, 16, 12), // "fileSelected"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 4), // "path"
QT_MOC_LITERAL(4, 35, 10), // "selectFile"
QT_MOC_LITERAL(5, 46, 11), // "updateLabel"
QT_MOC_LITERAL(6, 58, 11), // "QModelIndex"
QT_MOC_LITERAL(7, 70, 5), // "index"
QT_MOC_LITERAL(8, 76, 16) // "loadTextFromFile"

    },
    "Load_FileDialog\0fileSelected\0\0path\0"
    "selectFile\0updateLabel\0QModelIndex\0"
    "index\0loadTextFromFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Load_FileDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   37,    2, 0x0a /* Public */,
       5,    1,   38,    2, 0x0a /* Public */,
       8,    1,   41,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void Load_FileDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Load_FileDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->fileSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->selectFile(); break;
        case 2: _t->updateLabel((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 3: _t->loadTextFromFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Load_FileDialog::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Load_FileDialog::fileSelected)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Load_FileDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<AppWindow::staticMetaObject>(),
    qt_meta_stringdata_Load_FileDialog.data,
    qt_meta_data_Load_FileDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Load_FileDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Load_FileDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Load_FileDialog.stringdata0))
        return static_cast<void*>(this);
    return AppWindow::qt_metacast(_clname);
}

int Load_FileDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AppWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Load_FileDialog::fileSelected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
