######################################################################
# Automatically generated by qmake (1.07a) Sat May 22 11:40:57 2004
######################################################################

TEMPLATE = lib
INCLUDEPATH += .

# Input
HEADERS += common.h library_extragui_init.h qt_accessors.h
SOURCES += library_extragui_init.cpp qt_accessors.cpp

TARGET=GsTLAppli_gui



contains( TEMPLATE, lib ) {
    DESTDIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM/obj/
}
contains( TEMPLATE, app ) {
    DESTDIR += $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM/obj/
}


include( $$GSTLAPPLI_HOME/config.qmake )

exists( MMakefile ) {
   include( MMakefile )
}
