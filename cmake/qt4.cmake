# -*- cmake -*-

set( LIBRARIES QtCore QtXml )
#if( NOT MO_WINDOWS )
#list( APPEND LIBRARIES QtDBus )
#endif()

find_package( Qt4 4.7.4 REQUIRED ${LIBRARIES} )
#set( QT_USE_PHONON FALSE )
#set( QT_USE_QTMAIN TRUE )
#set( QT_USE_QTCORE TRUE )
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )
