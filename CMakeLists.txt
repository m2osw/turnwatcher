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
cmake_minimum_required( VERSION 2.8 )

project( turnwatcher_project )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"   )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"   )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"   )
set( CMAKE_MODULE_PATH              "${CMAKE_SOURCE_DIR}/cmake" )

include( 00-Common )

#enable_testing()

add_subdirectory( src )


########### TODO: install files (?) ###############

## vim: ts=4 sw=4 noexpandtab
