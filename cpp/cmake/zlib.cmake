find_file( ZLIB_INCLUDE_FILE zlib.h HINTS /usr/include )
find_library( ZLIB_LIBRARY zlib HINTS /usr/lib )

if( ${ZLIB_INCLUDE_FILE} STREQUAL ZLIB_INCLUDE_FILE-NOTFOUND )
	message( FATAL_ERROR "Cannot locate zlib.h. Is libtiff5 installed?" )
endif()
get_filename_component( ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_FILE} PATH )

if( ${ZLIB_LIBRARY} STREQUAL ZLIB_LIBRARY-NOTFOUND )
	message( FATAL_ERROR "Cannot locate zlib library. Is zlib installed?" )
endif()

