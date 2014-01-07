/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Thu Aug 30 16:09:25 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      27,   11,   11,   11, 0x0a,
      40,   11,   11,   11, 0x0a,
      51,   11,   11,   11, 0x0a,
      64,   11,   11,   11, 0x0a,
      74,   11,   11,   11, 0x0a,
      85,   11,   11,   11, 0x0a,
      98,   11,   11,   11, 0x0a,
     111,  108,   11,   11, 0x0a,
     140,   11,   11,   11, 0x0a,
     157,   11,   11,   11, 0x0a,
     165,   11,   11,   11, 0x0a,
     184,   11,   11,   11, 0x0a,
     195,   11,   11,   11, 0x0a,
     207,   11,   11,   11, 0x0a,
     225,   11,   11,   11, 0x0a,
     240,   11,   11,   11, 0x0a,
     258,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0countSpectra()\0setExpName()\0"
    "sliceAll()\0setEdfFile()\0cutShow()\0"
    "makeShow()\0makePaSlot()\0netShow()\0,,\0"
    "takeSpValues(int,int,double)\0"
    "reduceChannels()\0setNs()\0drawRealisations()\0"
    "eyesShow()\0cleanDirs()\0changeNsLine(int)\0"
    "waveletCount()\0setBoxMax(double)\0"
    "makeDatFile()\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->countSpectra(); break;
        case 1: _t->setExpName(); break;
        case 2: _t->sliceAll(); break;
        case 3: _t->setEdfFile(); break;
        case 4: _t->cutShow(); break;
        case 5: _t->makeShow(); break;
        case 6: _t->makePaSlot(); break;
        case 7: _t->netShow(); break;
        case 8: _t->takeSpValues((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 9: _t->reduceChannels(); break;
        case 10: _t->setNs(); break;
        case 11: _t->drawRealisations(); break;
        case 12: _t->eyesShow(); break;
        case 13: _t->cleanDirs(); break;
        case 14: _t->changeNsLine((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->waveletCount(); break;
        case 16: _t->setBoxMax((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 17: _t->makeDatFile(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
