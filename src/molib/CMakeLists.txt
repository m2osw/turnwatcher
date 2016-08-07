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

add_subdirectory( generators )

set( MO_CONTROLLED_H
        ${CMAKE_CURRENT_BINARY_DIR}/mo_controlled.h
        )
set_source_files_properties( ${MO_CONTROLLED_H} PROPERTIES GENERATED TRUE )
add_custom_command(	OUTPUT ${MO_CONTROLLED_H}
					COMMAND controlled_vars > ${MO_CONTROLLED_H}
					WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
					COMMENT "Generating ${MO_CONTROLLED_H}"
					)
add_custom_target( mo_controlled_h ALL DEPENDS ${MO_CONTROLLED_H} )

#configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )
#add_definitions( -DHAVE_CONFIG )

project( molib )

if( MO_WINDOWS )
	include( iconv )
	include( jpeglib )
	include( regex )
#	include( tiff  )
	include( tar   )
	include( zlib  )
    add_definitions( -DMO_WIN32 )
	include_directories(
		${PROJECT_SOURCE_DIR}/include/win32
		${ICONV_INCLUDE_DIR}
		${JPEGLIB_INCLUDE_DIR}
		${REGEX_INCLUDE_DIR}
#		${TIFF_INCLUDE_DIR}
		${TAR_INCLUDE_DIR}
		${ZLIB_INCLUDE_DIR}
		)
endif()

include_directories(
		${PROJECT_SOURCE_DIR}/include
		)

set( HEADERS_DIR include/mo )
set( HEADERS
		config.h.cmake
		${MO_CONTROLLED_H}
		${HEADERS_DIR}/details/mo_atomic.h
		${HEADERS_DIR}/mo_application.h
		${HEADERS_DIR}/mo_array.h
		${HEADERS_DIR}/mo_auto_restore.h
		${HEADERS_DIR}/mo_base.h
		${HEADERS_DIR}/mo_buffer.h
		${HEADERS_DIR}/mo_config.h
		${HEADERS_DIR}/mo_controlled.h
		${HEADERS_DIR}/mo_crypt.h
		${HEADERS_DIR}/mo_directory.h
		${HEADERS_DIR}/mo_dirent.h
		${HEADERS_DIR}/mo_error.h
		${HEADERS_DIR}/mo_event.h
		${HEADERS_DIR}/mo_export.h
		${HEADERS_DIR}/mo_expr.h
		${HEADERS_DIR}/mo_fifo.h
		${HEADERS_DIR}/mo_file.h
		${HEADERS_DIR}/mo_getopt.h
		${HEADERS_DIR}/mo_gzip.h
		${HEADERS_DIR}/mo_image.h
		${HEADERS_DIR}/mo_list.h
		${HEADERS_DIR}/mo_luhn.h
		${HEADERS_DIR}/mo_memfile.h
		${HEADERS_DIR}/mo_menu_item.h
		${HEADERS_DIR}/mo_menu_manager.h
		${HEADERS_DIR}/mo_module.h
		${HEADERS_DIR}/mo_mutex.h
		${HEADERS_DIR}/mo_name.h
		${HEADERS_DIR}/mo_passwd.h
		${HEADERS_DIR}/mo_process.h
		${HEADERS_DIR}/mo_props.h
		${HEADERS_DIR}/mo_props_xml.h
		${HEADERS_DIR}/mo_random.h
		${HEADERS_DIR}/mo_regexpr.h
		${HEADERS_DIR}/mo_simple_editor.h
		${HEADERS_DIR}/mo_smartptr.h
		${HEADERS_DIR}/mo_socket.h
		${HEADERS_DIR}/mo_stdint.h
		${HEADERS_DIR}/mo_stream.h
		${HEADERS_DIR}/mo_str.h
		${HEADERS_DIR}/mo_string.h
		${HEADERS_DIR}/mo_tar.h
		${HEADERS_DIR}/mo_template.h
		${HEADERS_DIR}/mo_text_stream.h
		${HEADERS_DIR}/mo_thread.h
		${HEADERS_DIR}/mo_transaction.h
		${HEADERS_DIR}/mo_types.h
		${HEADERS_DIR}/mo_uuencode.h
		${HEADERS_DIR}/mo_variable.h
		${HEADERS_DIR}/mo_variable_reducer.h
		${HEADERS_DIR}/mo_variant.h
		${HEADERS_DIR}/mo_words.h
		${HEADERS_DIR}/mo_xml_basic.h
		${HEADERS_DIR}/mo_xml.h
	)

if( MO_WINDOWS )
	list( APPEND HEADERS ${HEADERS_DIR}/details/mo_win32_handle.h )
endif()

set( SOURCES_DIR src )
set( SOURCES
		${SOURCES_DIR}/application.cpp
		${SOURCES_DIR}/array.cpp
		${SOURCES_DIR}/base.cpp
		${SOURCES_DIR}/buffer.cpp
		${SOURCES_DIR}/crypt.cpp
		${SOURCES_DIR}/directory.cpp
		${SOURCES_DIR}/error.cpp
		${SOURCES_DIR}/event.cpp
		${SOURCES_DIR}/expr.cpp
		${SOURCES_DIR}/fifo.cpp
		${SOURCES_DIR}/file.cpp
		${SOURCES_DIR}/getopt.cpp
		${SOURCES_DIR}/gzip.cpp
		${SOURCES_DIR}/html_dtd.cpp
		${SOURCES_DIR}/image_bmp.cpp
		${SOURCES_DIR}/image.cpp
		${SOURCES_DIR}/image_file.cpp
		${SOURCES_DIR}/image_gif.cpp
		${SOURCES_DIR}/image_jpeg.cpp
		${SOURCES_DIR}/image_png.cpp
#		${SOURCES_DIR}/image_ppm.cpp
#		${SOURCES_DIR}/image_raw.cpp
#		${SOURCES_DIR}/image_sgi.cpp
#		${SOURCES_DIR}/image_targa.cpp
#		${SOURCES_DIR}/image_tiff.cpp
		${SOURCES_DIR}/list.cpp
		${SOURCES_DIR}/luhn.cpp
		${SOURCES_DIR}/memfile.cpp
		${SOURCES_DIR}/menu_item.cpp
		${SOURCES_DIR}/menu_manager.cpp
#		${SOURCES_DIR}/module.cpp
		${SOURCES_DIR}/mutex.cpp
		${SOURCES_DIR}/name.cpp
#		${SOURCES_DIR}/passwd.cpp
#		${SOURCES_DIR}/process.cpp
		${SOURCES_DIR}/props.cpp
		${SOURCES_DIR}/props_xml.cpp
		${SOURCES_DIR}/random.cpp
		${SOURCES_DIR}/regexpr.cpp
		${SOURCES_DIR}/simple_editor.cpp
#		${SOURCES_DIR}/socket.cpp
		${SOURCES_DIR}/str.cpp
		${SOURCES_DIR}/stream.cpp
		${SOURCES_DIR}/string.cpp
#		${SOURCES_DIR}/tar.cpp
		${SOURCES_DIR}/text_stream.cpp
		${SOURCES_DIR}/thread.cpp
		${SOURCES_DIR}/transaction.cpp
		${SOURCES_DIR}/uuencode.cpp
		${SOURCES_DIR}/variable.cpp
		${SOURCES_DIR}/variable_reducer.cpp
		${SOURCES_DIR}/words.cpp
		${SOURCES_DIR}/xml_basic.cpp
		${SOURCES_DIR}/xml.cpp
	)

add_library( ${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} )

target_link_libraries( ${PROJECT_NAME}
		${ICONV_LIBRARY}
		${JPEGLIB_LIBRARY}
#		${TIFF_LIBRARY}
		${REGEX_LIBRARY}
		${ZLIB_LIBRARY}
		)

# vim: ts=4 sw=4 noexpandtab
