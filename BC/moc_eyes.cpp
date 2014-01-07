/****************************************************************************
** Meta object code from reading C++ file 'eyes.h'
**
** Created: Tue Aug 28 20:34:23 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "eyes.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'eyes.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Eyes[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       6,    5,    5,    5, 0x0a,
      23,    5,    5,    5, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Eyes[] = {
    "Eyes\0\0eyesProcessing()\0eyesClean()\0"
};

void Eyes::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Eyes *_t = static_cast<Eyes *>(_o);
        switch (_id) {
        case 0: _t->eyesProcessing(); break;
        case 1: _t->eyesClean(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Eyes::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Eyes::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Eyes,
      qt_meta_data_Eyes, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Eyes::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Eyes::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Eyes::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Eyes))
        return static_cast<void*>(const_cast< Eyes*>(this));
    return QWidget::qt_metacast(_clname);
}

int Eyes::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
