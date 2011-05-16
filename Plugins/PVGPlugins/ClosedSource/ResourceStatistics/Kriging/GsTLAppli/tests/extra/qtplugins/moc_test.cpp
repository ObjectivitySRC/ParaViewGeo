/****************************************************************************
** Form1 meta object code from reading C++ file 'test.h'
**
** Created: Thu Oct 16 23:01:59 2003
**      by: The Qt MOC ($Id: moc_test.cpp,v 4.0 2008/04/23 02:42:13 nico97492 Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "test.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.1.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Form1::className() const
{
    return "Form1";
}

QMetaObject *Form1::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Form1( "Form1", &Form1::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Form1::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Form1", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Form1::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Form1", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Form1::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"Form1", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Form1.setMetaObject( metaObj );
    return metaObj;
}

void* Form1::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Form1" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool Form1::qt_invoke( int _id, QUObject* _o )
{
    return QDialog::qt_invoke(_id,_o);
}

bool Form1::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Form1::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool Form1::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
