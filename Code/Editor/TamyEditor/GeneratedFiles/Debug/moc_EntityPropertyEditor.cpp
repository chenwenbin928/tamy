/****************************************************************************
** Meta object code from reading C++ file 'EntityPropertyEditor.h'
**
** Created: Sat 24. Apr 18:21:09 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../EntityPropertyEditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EntityPropertyEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EntityPropertyEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_EntityPropertyEditor[] = {
    "EntityPropertyEditor\0"
};

const QMetaObject EntityPropertyEditor::staticMetaObject = {
    { &QPropertyEditor::staticMetaObject, qt_meta_stringdata_EntityPropertyEditor,
      qt_meta_data_EntityPropertyEditor, 0 }
};

const QMetaObject *EntityPropertyEditor::metaObject() const
{
    return &staticMetaObject;
}

void *EntityPropertyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EntityPropertyEditor))
        return static_cast<void*>(const_cast< EntityPropertyEditor*>(this));
    return QPropertyEditor::qt_metacast(_clname);
}

int EntityPropertyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPropertyEditor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
