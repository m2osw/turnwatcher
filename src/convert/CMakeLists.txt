project( tw_convert )

include( qt4 )

set( HEADERS
	Variant.h
	XmlVariant.h
	)
set( SOURCES
	Variant.cpp
	XmlVariant.cpp
   )

add_library( ${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} )

target_link_libraries( ${PROJECT_NAME} ${QT_LIBRARIES} )

# vim: ts=4 sw=4 noexpandtab
