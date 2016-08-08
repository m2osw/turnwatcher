find_file( TIFF_INCLUDE_FILE tiffio.h HINTS /usr/include )
find_library( TIFF_LIBRARY tiff HINTS /usr/lib )

if( ${TIFF_INCLUDE_FILE} STREQUAL TIFF_INCLUDE_FILE-NOTFOUND )
	message( FATAL_ERROR "Cannot locate tiffio.h. Is libtiff5 installed?" )
endif()
get_filename_component( TIFF_INCLUDE_DIR ${TIFF_INCLUDE_FILE} PATH )

if( ${TIFF_LIBRARY} STREQUAL TIFF_LIBRARY-NOTFOUND )
	message( FATAL_ERROR "Cannot locate tiff library. Is libtiff5 installed?" )
endif()

