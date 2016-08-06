# - Try to find sigc++
#
# Once done this will define
#
# SIGCPP_FOUND        - System has sigc++
# SIGCPP_INCLUDE_DIR  - The sigc++ include directories
# SIGCPP_LIBRARY      - The libraries needed to use sigc++ (none)
# SIGCPP_DEFINITIONS  - Compiler switches required for using sigc++ (none)

find_path( SIGCPP_INCLUDE_DIR sigc++
  HINTS $ENV{SIGCPP_INCLUDE_DIR}
  PATH_SUFFIXES sigc++
  PATHS
	  /usr/include/sigc++-2.0
	  /usr/local/include
	  /opt/local/include
	  /sw
)

find_library( SIGCPP_LIBRARY
  NAMES sigc-2.0 sigc libsigc-2.0 libsigc
  HINTS $ENV{SIGCPP_LIBRARY_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
	  /usr/lib/x86_64-linux-gnu/
	  /usr/lib/i386-linux-gnu/
	  /usr/local/lib
	  /opt/local/lib
	  /sw
)

include( FindPackageHandleStandardArgs )
# handle the QUIETLY and REQUIRED arguments and set SIGCPP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args( SigCpp DEFAULT_MSG SIGCPP_INCLUDE_DIR SIGCPP_LIBRARY )

mark_as_advanced( SIGCPP_INCLUDE_DIR SIGCPP_LIBRARY )

set( SIGC_INCLUDE_DIRS ${SIGCPP_INCLUDE_DIR} )
set( SIGC_LIBRARIES    ${SIGCPP_LIBRARY}     )

