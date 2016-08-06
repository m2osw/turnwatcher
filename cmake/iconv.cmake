find_file( ICONV_INCLUDE_FILE iconv.h PATH /usr/include )
if( MO_WINDOWS AND ${ICONV_INCLUDE_FILE} STREQUAL ICONV_INCLUDE_FILE-NOTFOUND )
	message( FATAL_ERROR "Cannot locate iconv.h. Is libtiff5 installed?" )
endif()
get_filename_component( ICONV_INCLUDE_DIR ${ICONV_INCLUDE_FILE} PATH )

find_library( ICONV_LIBRARY iconv PATH /usr/lib )
if( MO_WINDOWS AND ${ICONV_LIBRARY} STREQUAL ICONV_LIBRARY-NOTFOUND )
	message( FATAL_ERROR "Cannot locate iconv library. Is iconv installed?" )
endif()

