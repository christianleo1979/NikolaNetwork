/****************************************************************************
** Meta object code from reading C++ file 'NW_BaseAnalogInput.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/NW_BaseAnalogInput.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NW_BaseAnalogInput.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NW_BaseAnalogInput[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   20,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      60,   48,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NW_BaseAnalogInput[] = {
    "NW_BaseAnalogInput\0\0,\0onValueChanged(long,long)\0"
    "id,newValue\0update(long,long)\0"
};

void NW_BaseAnalogInput::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NW_BaseAnalogInput *_t = static_cast<NW_BaseAnalogInput *>(_o);
        switch (_id) {
        case 0: _t->onValueChanged((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2]))); break;
        case 1: _t->update((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NW_BaseAnalogInput::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NW_BaseAnalogInput::staticMetaObject = {
    { &QSlider::staticMetaObject, qt_meta_stringdata_NW_BaseAnalogInput,
      qt_meta_data_NW_BaseAnalogInput, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NW_BaseAnalogInput::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NW_BaseAnalogInput::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NW_BaseAnalogInput::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NW_BaseAnalogInput))
        return static_cast<void*>(const_cast< NW_BaseAnalogInput*>(this));
    if (!strcmp(_clname, "INikolaWidget"))
        return static_cast< INikolaWidget*>(const_cast< NW_BaseAnalogInput*>(this));
    if (!strcmp(_clname, "IKeyboardSupport"))
        return static_cast< IKeyboardSupport*>(const_cast< NW_BaseAnalogInput*>(this));
    if (!strcmp(_clname, "IMouseSupport"))
        return static_cast< IMouseSupport*>(const_cast< NW_BaseAnalogInput*>(this));
    return QSlider::qt_metacast(_clname);
}

int NW_BaseAnalogInput::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSlider::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void NW_BaseAnalogInput::onValueChanged(long _t1, long _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
