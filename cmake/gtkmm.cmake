include( freetype  )
include( FindPkgConfig )

pkg_check_modules( GTKMM REQUIRED gtkmm-2.4 )
if( NOT MO_WINDOWS )
	pkg_check_modules( WEBKIT REQUIRED webkit-1.0 )
	list( APPEND GTKMM_INCLUDE_DIRS ${WEBKIT_INCLUDE_DIRS} )
endif()

if( MO_WINDOWS )
    find_path( GTKMM_ROOT_DIR gtkmm PATH c:/gtkmm )
    if( NOT ${GTKMM_ROOT_DIR} STREQUAL "GTKMM_ROOT_DIR-NOTFOUND" )
        list( APPEND GTKMM_INCLUDE_DIRS ${GTKMM_ROOT_DIR}/include )
    endif()
endif()

