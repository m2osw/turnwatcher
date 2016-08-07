# -*- cmake -*-

find_path( SIGCPP_INCLUDE_DIR sigc++
  HINTS
  $ENV{SIGCPP_INCLUDE_DIR}
  PATHS
	  /usr/include/sigc++-2.0
	  /usr/local/include
	  /opt/local/include
	  /sw
)

find_library( SIGCPP_LIBRARY
  NAMES sigc-2.0 sigc libsigc-2.0 libsigc
  HINTS
  $ENV{SIGCPP_LIBRARY_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
	  /usr/lib/x86_64-linux-gnu/
	  /usr/lib/i386-linux-gnu/
	  /usr/local/lib
	  /opt/local/lib
	  /sw
)

set( SIGC_INCLUDE_DIRS ${SIGCPP_INCLUDE_DIR} )
set( SIGC_LIBRARIES    ${SIGCPP_LIBRARY}     )
