include( freetype  )
include( FindPkgConfig )

#pkg_check_modules( GTKMM pangomm-1.4 glib-2.0 gdk-pixbuf-2.0 gtk2-2.0 glibmm-2.0 gtkmm-2.0 )
pkg_check_modules( GTKMM REQUIRED gtkmm-2.4 )
pkg_check_modules( WEBKIT REQUIRED webkit-1.0 )
list( APPEND GTKMM_INCLUDE_DIRS ${WEBKIT_INCLUDE_DIRS} )
#message( "GTKMM_INCLUDE_DIRS=${GTKMM_INCLUDE_DIRS}" )

#message( "GTKMM_INCLUDE_DIRS=${GTKMM_INCLUDE_DIRS}" )
#message( "GTKMM_LIBRARIES=${GTKMM_LIBRARIES}" )

#find_package( GTK2 2.8 COMPONENTS gtk gtkmm )
#if( NOT GTK2_FOUND )
#	message( FATAL_ERROR "Freetype library not found!" )
#endif()
#list( APPEND GTKMM_INCLUDE_DIRS ${GTK2_INCLUDE_DIRS} )
#list( APPEND GTKMM_LIBRARIES    ${GTK2_LIBRARIES}    )

