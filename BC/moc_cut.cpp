/****************************************************************************
** Meta object code from reading C++ file 'cut.h'
**
** Created: Thu Aug 30 15:58:17 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "cut.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cut.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Cut[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
       5,    4,    4,    4, 0x05,
      33,   23,    4,    4, 0x05,

 // slots: signature, parameters, type, tag, flags
      61,   57,    4,    4, 0x0a,
      96,   82,    4,    4, 0x0a,
     121,    4,    4,    4, 0x0a,
     128,    4,    4,    4, 0x0a,
     135,    4,    4,    4, 0x0a,
     141,    4,    4,    4, 0x0a,
     148,    4,    4,    4, 0x0a,
     156,    4,    4,    4, 0x0a,
     163,    4,    4,    4, 0x0a,
     174,    4,    4,    4, 0x0a,
     187,    4,    4,    4, 0x0a,
     202,    4,    4,    4, 0x0a,
     219,    4,    4,    4, 0x0a,
     236,    4,    4,    4, 0x0a,
     247,    4,    4,    4, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Cut[] = {
    "Cut\0\0openFile(QString)\0btn,coord\0"
    "buttonPressed(char,int)\0str\0"
    "createImage(QString)\0btn__,coord__\0"
    "mousePressSlot(char,int)\0next()\0prev()\0"
    "cut()\0zero()\0paint()\0save()\0lnWndAll()\0"
    "paintDistr()\0drawLogistic()\0"
    "cycleTimeShift()\0cycleWndLength()\0"
    "setNs(int)\0cutEyesAll()\0"
};

void Cut::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Cut *_t = static_cast<Cut *>(_o);
        switch (_id) {
        case 0: _t->openFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->buttonPressed((*reinterpret_cast< char(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->createImage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->mousePressSlot((*reinterpret_cast< char(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->next(); break;
        case 5: _t->prev(); break;
        case 6: _t->cut(); break;
        case 7: _t->zero(); break;
        case 8: _t->paint(); break;
        case 9: _t->save(); break;
        case 10: _t->lnWndAll(); break;
        case 11: _t->paintDistr(); break;
        case 12: _t->drawLogistic(); break;
        case 13: _t->cycleTimeShift(); break;
        case 14: _t->cycleWndLength(); break;
        case 15: _t->setNs((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->cutEyesAll(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Cut::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Cut::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Cut,
      qt_meta_data_Cut, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Cut::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Cut::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Cut::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Cut))
        return static_cast<void*>(const_cast< Cut*>(this));
    return QWidget::qt_metacast(_clname);
}

int Cut::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void Cut::openFile(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Cut::buttonPressed(char _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
