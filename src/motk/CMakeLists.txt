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

include( gtkmm )

macro( motk_AddEvents )
	foreach( motk_file ${ARGN} )
		get_filename_component( motk_file ${motk_file} ABSOLUTE )
		get_filename_component( motk_basefile ${motk_file} NAME_WE )
		add_custom_command(    
				OUTPUT ${PROJECT_BINARY_DIR}/${motk_basefile}.c++ ${PROJECT_BINARY_DIR}/${motk_basefile}.h
				COMMAND async_functions
				ARGS ${motk_file}
				DEPENDS ${motk_file}
				WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
				COMMENT "Parsing event file ${motk_file} and creating ${motk_basefile}.h/c++"
				)
		list( APPEND HEADERS ${PROJECT_BINARY_DIR}/${motk_basefile}.h   )
		list( APPEND SOURCES ${PROJECT_BINARY_DIR}/${motk_basefile}.c++ )
	endforeach()
endmacro()

if( MO_WINDOWS )
	include( iconv )
	include( tar )
	include_directories(
		${molib_SOURCE_DIR}/include/win32
        ${ICONV_INCLUDE_DIR}
		${TAR_INCLUDE_DIR}
	)
endif()

project( motk )

include_directories(
		${molib_SOURCE_DIR}/include
		${molib_SOURCE_DIR}/include/mo
		${PROJECT_SOURCE_DIR}
		${PROJECT_BINARY_DIR}
		${GTKMM_INCLUDE_DIRS}
		)

set( HEADERS
		ActionManager.h
		Chooser.h
		Common.h
		HtmlWidget.h
		LabelTable.h
		LabelTableScroller.h
		MenuManager.h
		UiBase.h
	)

set( SOURCES
		ActionManager.cpp
		Chooser.cpp
		HtmlWidget.cpp
		LabelTable.cpp
		LabelTableScroller.cpp
		MenuManager.cpp
		UiBase.cpp
	)

motk_AddEvents( Menu_event.async )
add_library( ${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} )
target_link_libraries( ${PROJECT_NAME}
        ${ICONV_INCLUDE_DIR}
        ${GTKMM_LIBRARIES}
    )
target_compile_features( ${PROJECT_NAME} PUBLIC
        cxx_deleted_functions
        cxx_defaulted_functions
        cxx_noexcept
        cxx_lambdas
        cxx_range_for
        cxx_override
    )

# vim: ts=4 sw=4 noexpandtab
