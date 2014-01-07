/****************************************************************************
** Meta object code from reading C++ file 'net.h'
**
** Created: Tue Aug 28 20:34:21 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "net.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'net.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Net[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       5,    4,    4,    4, 0x0a,
      15,    4,    4,    4, 0x0a,
      25,    4,    4,    4, 0x0a,
      36,    4,    4,    4, 0x0a,
      43,    4,    4,    4, 0x0a,
      51,    4,    4,    4, 0x0a,
      61,    4,    4,    4, 0x0a,
      73,    4,    4,    4, 0x0a,
      80,    4,    4,    4, 0x0a,
      87,    4,    4,    4, 0x0a,
      94,    4,    4,    4, 0x0a,
     104,    4,    4,    4, 0x0a,
     118,    4,    4,    4, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Net[] = {
    "Net\0\0readCfg()\0loadWts()\0LearnNet()\0"
    "tall()\0reset()\0saveWts()\0stopLearn()\0"
    "test()\0next()\0prev()\0drawWts()\0"
    "compressWts()\0PaIntoMatrix()\0"
};

void Net::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Net *_t = static_cast<Net *>(_o);
        switch (_id) {
        case 0: _t->readCfg(); break;
        case 1: _t->loadWts(); break;
        case 2: _t->LearnNet(); break;
        case 3: _t->tall(); break;
        case 4: _t->reset(); break;
        case 5: _t->saveWts(); break;
        case 6: _t->stopLearn(); break;
        case 7: _t->test(); break;
        case 8: _t->next(); break;
        case 9: _t->prev(); break;
        case 10: _t->drawWts(); break;
        case 11: _t->compressWts(); break;
        case 12: _t->PaIntoMatrix(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Net::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Net::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Net,
      qt_meta_data_Net, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Net::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Net::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Net::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Net))
        return static_cast<void*>(const_cast< Net*>(this));
    return QWidget::qt_metacast(_clname);
}

int Net::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
