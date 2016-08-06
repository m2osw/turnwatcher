find_file( REGEX_INCLUDE_FILE regex.h HINTS /usr/include )
find_library( REGEX_LIBRARY regex HINTS /usr/lib )

if( ${REGEX_INCLUDE_FILE} STREQUAL REGEX_INCLUDE_FILE-NOTFOUND )
        message( FATAL_ERROR "Cannot locate regex.h. Is regex installed?" )
endif()
get_filename_component( REGEX_INCLUDE_DIR ${REGEX_INCLUDE_FILE} PATH )

if( ${REGEX_LIBRARY} STREQUAL REGEX_LIBRARY-NOTFOUND )
        message( FATAL_ERROR "Cannot locate regex library. Is regex lib installed?" )
endif()
