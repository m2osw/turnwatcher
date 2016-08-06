##===============================================================================
## Copyright (c) 2005-2013 by Made to Order Software Corporation
## 
## All Rights Reserved.
## 
## The source code in this file ("Source Code") is provided by Made to Order Software Corporation
## to you under the terms of the GNU General Public License, version 2.0
## ("GPL").  Terms of the GPL can be found in doc/GPL-license.txt in this distribution.
## 
## By copying, modifying or distributing this software, you acknowledge
## that you have read and understood your obligations described above,
## and agree to abide by those obligations.
## 
## ALL SOURCE CODE IN THIS DISTRIBUTION IS PROVIDED "AS IS." THE AUTHOR MAKES NO
## WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
## COMPLETENESS OR PERFORMANCE.
##===============================================================================

project( package )

############################################################
## Package defs

set( CPACK_PACKAGE_NAME "${BIN_NAME}" )
if( DEMO_VERSION )
	set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Turn Watcher Demo Version" )
else()
	set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Turn Watcher" )
endif()
set( CPACK_PACKAGE_VENDOR              "Made to Order Software Corporation"  					)
set( CPACK_PACKAGE_CONTACT             "contact@m2osw.com"                   					)
set( CPACK_PACKAGE_DESCRIPTION_FILE    "${CMAKE_SOURCE_DIR}/README"          					)
set( CPACK_RESOURCE_FILE_LICENSE       "${CMAKE_SOURCE_DIR}/doc/LICENSE.txt" 					)
set( CPACK_PACKAGE_VERSION_MAJOR       ${turnwatcher_VERSION_MAJOR}          					)
set( CPACK_PACKAGE_VERSION_MINOR       ${turnwatcher_VERSION_MINOR}          					)
set( CPACK_PACKAGE_VERSION_PATCH       ${turnwatcher_VERSION_PATCH}          					)
set( CPACK_PACKAGE_EXECUTABLES         "${BIN_NAME}" "Game Master Initiative Manager and More!"	)
#
set( CPACK_STRIP_FILES TRUE )


if( MO_WINDOWS )
	set( CPACK_GENERATOR "NSIS" )
	if( DEMO_VERSION )
		set( CPACK_PACKAGE_INSTALL_DIRECTORY "Turn Watcher Demo" )
	else()
		set( CPACK_PACKAGE_INSTALL_DIRECTORY "Turn Watcher" )
	endif()
	set( CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/src/pixmaps\\\\turnwatcher.png" )
	set( CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\\\\bin/${BIN_NAME}.exe" )
	set( CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} Turn Watcher for Windows" )
	set( CPACK_NSIS_HELP_LINK "http://www.turnwatcher.com/" )
	set( CPACK_NSIS_URL_INFO_ABOUT "http://www.turnwatcher.com/" )
	set( CPACK_NSIS_CONTACT "contact@m2osw.com" )
	set( CPACK_NSIS_MODIFY_PATH ON )
else( LINUX )
	set( CPACK_GENERATOR "DEB;TGZ" )
	set( CPACK_PACKAGE_INSTALL_DIRECTORY "${BIN_NAME}" )
	set( CPACK_PACKAGE_FILE_NAME "${BIN_NAME}-${turnwatcher_VERSION_MAJOR}.${turnwatcher_VERSION_MINOR}.${turnwatcher_VERSION_PATCH}-Linux-${CMAKE_SYSTEM_PROCESSOR}" )
	set( CPACK_DEBIAN_PACKAGE_DEPENDS "libatk1.0-0 (>= 1.30.0-0ubuntu2.1), libc6 (>= 2.11.1-0ubuntu7.8), libcairo2 (>= 1.8.10-2ubuntu1), libcairomm-1.0-1 (>= 1.8.4-0ubuntu1), libgcc1 (>= 1:4.4.3-4ubuntu5), libglib2.0-0 (>= 2.24.1-0ubuntu1), libglibmm-2.4-1c2a (>= 2.24.2-0ubuntu1), libgtk2.0-0 (>= 2.20.1-0ubuntu2), libgtkmm-2.4-1c2a (>= 1:2.20.3-0ubuntu1), libpango1.0-0 (>= 1.28.0-0ubuntu2.1), libpangomm-1.4-1 (>= 2.26.2-0ubuntu1), libsigc++-2.0-0c2a (>= 2.2.4.2-1), libstdc++6 (>= 4.4.3-4ubuntu5)" )
	set( CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/dev/linux/postinst;${CMAKE_SOURCE_DIR}/dev/linux/postrm;" )
else()
	message( FATAL_ERROR "Only Windows and Linux supported right now..." )
endif()

include( CPack )

# vim: ts=4 sw=4 noexpandtab
