# - Try to find gtkmm
#
# Once done this will define
#
# GTKMM_FOUND        - System has gtkmm
# GTKMM_INCLUDE_DIRS  - The gtkmm include directories
# GTKMM_LIBRARIES     - The libraries needed to use gtkmm (none)
# GTKMM_DEFINITIONS  - Compiler switches required for using gtkmm (none)

if( MO_LINUX )
	find_package( PkgConfig ) #GTKMM REQUIRED gtkmm-3.0 )
	pkg_check_modules( GTKMM REQUIRED gtkmm-3.0 )
	set( GTKMM_DEFINITIONS ${GTKMM_CFLAGS} ${GTKMM_CFLAGS_OTHER} )
else()
	find_path( GTKMM_INCLUDE_DIR gtkmm
	  HINTS $ENV{GTKMM_INCLUDE_DIR}
	  PATH_SUFFIXES gtkmm
	  PATHS
		  /usr/include/gtkmm-3.0
		  /usr/local/include
		  /opt/local/include
		  /sw
	)

	find_path( GLIBMM_INCLUDE_DIR glibmm
	  HINTS $ENV{GLIBMM_INCLUDE_DIR}
	  PATH_SUFFIXES glibmm
	  PATHS
		  /usr/include/glibmm-2.4
		  /usr/local/include
		  /opt/local/include
		  /sw
	)

	find_library( GTKMM_LIBRARY
	  NAMES gtkmm-3.0 gtkmm
	  HINTS $ENV{GTKMM_LIBRARY_DIR}
	  PATH_SUFFIXES lib64 lib
	  PATHS
		  /usr/lib/x86_64-linux-gnu/
		  /usr/lib/i386-linux-gnu/
		  /usr/local/lib
		  /opt/local/lib
		  /sw
	)

	find_library( GLIBMM_LIBRARY
	  NAMES glibmm-3.0 glibmm
	  HINTS $ENV{GLIBMM_LIBRARY_DIR}
	  PATH_SUFFIXES lib64 lib
	  PATHS
		  /usr/lib/x86_64-linux-gnu/
		  /usr/lib/i386-linux-gnu/
		  /usr/local/lib
		  /opt/local/lib
		  /sw
	)

	set( GTKMM_INCLUDE_DIRS ${GTKMM_INCLUDE_DIR} ${GLIBMM_INCLUDE_DIR} )
	set( GTKMM_LIBRARIES    ${GTKMM_LIBRARY_DIR} ${GLIBMM_LIBRARY_DIR} )
	#mark_as_advanced( GTKMM_INCLUDE_DIR GTKMM_LIBRARY )
endif()

include( FindPackageHandleStandardArgs )
# handle the QUIETLY and REQUIRED arguments and set GTKMM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args( GtkMM DEFAULT_MSG GTKMM_INCLUDE_DIRS GTKMM_LIBRARIES )

