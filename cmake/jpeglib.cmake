find_file( JPEGLIB_INCLUDE_FILE jpeglib.h HINTS /usr/include )
find_library( JPEGLIB_LIBRARY jpeglib HINTS /usr/lib )

if( ${JPEGLIB_INCLUDE_FILE} STREQUAL JPEGLIB_INCLUDE_FILE-NOTFOUND )
        message( FATAL_ERROR "Cannot locate jpeglib.h. Is jpeglib installed?" )
endif()
get_filename_component( JPEGLIB_INCLUDE_DIR ${JPEGLIB_INCLUDE_FILE} PATH )

if( ${JPEGLIB_LIBRARY} STREQUAL JPEGLIB_LIBRARY-NOTFOUND )
        message( FATAL_ERROR "Cannot locate jpeglib library. Is jpeglib installed?" )
endif()
