# -*- cmake -*-
#
# Detect the OS we are on, and the compiler features we are using.
#

# Portable compilation flags.
#
set( CMAKE_CXX_FLAGS_DEBUG          "-DDEBUG -D_DEBUG -DMO_DEBUG=1" )
set( CMAKE_CXX_FLAGS_RELEASE        "-DNDEBUG"                      )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-DNDEBUG"                      )

#===============================================================================
# Determine which platform we are on and set flags accordingly
#
if( UNIX )
	#
	# Linux
	#
	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set( MO_LINUX ON BOOL FORCE)
		execute_process(COMMAND uname -m COMMAND sed s/i.86/i686/
			OUTPUT_VARIABLE ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
		if( ${ARCH} MATCHES "i686" )
			set( MO_ARCH_SUFFIX        32     )
			set( MO_CANONALIZED_SUFFIX "i386" ) # for wpkg
		elseif( ${ARCH} MATCHES "x86_64" )
			set( MO_ARCH_SUFFIX        64      )
			set( MO_CANONALIZED_SUFFIX "amd64" ) # for wpkg
		else()
			message( FATAL_ERROR "Architecture '${ARCH}' unknown!" )
		endif()
		set( MO_ARCH_PREFIX        linux )
		set( MO_CANONALIZED_PREFIX linux )
		set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -g3 -O0 " )
		set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )
	endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

	# Darwin
	#
	if( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
		set( MO_DARWIN ON BOOL FORCE )
		# set this dynamically from the build system now -
		# NOTE: wont have a distributable build unless you add this on the configure line with:
		# -DCMAKE_OSX_ARCHITECTURES:STRING='i386;ppc'
		set( CMAKE_OSX_ARCHITECTURES i386 CACHE STRING "Supported DARWIN system (i386;ppc)" )
		set( CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX10.4u.sdk )
		if( (CMAKE_OSX_ARCHITECTURES MATCHES "i386") AND (CMAKE_OSX_ARCHITECTURES MATCHES "ppc") )
			set( MO_ARCH_SUFFIX universal )
		else()
			if( ${CMAKE_SYSTEM_PROCESSOR} MATCHES "ppc" )
				set( MO_ARCH_SUFFIX        ppc   )
				set( MO_CANONALIZED_SUFFIX "ppc" ) # for wpkg
			else()
				set( MO_ARCH_SUFFIX        i386   )
				set( MO_CANONALIZED_SUFFIX "i386" ) # for wpkg
			endif()
		endif()
		set( MO_ARCH_PREFIX        darwin )
		set( MO_CANONALIZED_PREFIX darwin )
	endif()
else()
	#
	# Microsoft Windows-land...
	#
	if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
		set( MO_WINDOWS ON BOOL FORCE )
		if( CMAKE_CL_64 )
			set( MO_ARCH_SUFFIX        64    )
			set( MO_CANONALIZED_SUFFIX amd64 )
		else()
			set( MO_ARCH_SUFFIX        32    )
			set( MO_CANONALIZED_SUFFIX i386  )
		endif()
		if( MSVC )
			set( MO_MSVC ON BOOL FORCE )
		elseif( NOT BORLAND AND NOT CYGWIN )
			set( MO_MINGW32 ON BOOL FORCE )
		else()
			message( FATAL_ERROR "This MS Windows platform is not supported! Use either MSVC or MinGW32." )
		endif()
		if( "${CMAKE_MAKE_PROGRAM}" MATCHES "nmake" )
			set( MO_USING_NMAKE ON BOOL FORCE )
		endif()
	endif()
endif()

set( MO_ARCH               ${MO_ARCH_PREFIX}${MO_ARCH_SUFFIX}                )
set( MO_CANONALIZED_ARCH   ${MO_CANONALIZED_PREFIX}-${MO_CANONALIZED_SUFFIX} )

#===============================================================================
# Platform-specfic features
#
if( MO_WINDOWS )
	set( CMAKE_CONFIGURATION_TYPES "RelWithDebInfo;Release;Debug" CACHE STRING "Supported build types." FORCE )

	if( MO_USING_NMAKE )
		string( REGEX REPLACE "/W[0-3]" "/W4"  CXX_FLAGS "${CMAKE_CXX_FLAGS}" )
		string( REGEX REPLACE "/EHs"    "/EHa" CXX_FLAGS "${CMAKE_CXX_FLAGS}" )
	else()
		set( CXX_FLAGS ${CMAKE_CXX_FLAGS} /W4 /EHa )
	endif()
	set( CMAKE_CXX_FLAGS ${CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS )

	set( CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /MDd"                  )
	set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Od /Zi /MD"          )
	set( CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} ${MO_CXX_FLAGS} /O2 /Zi /MD" )

	add_definitions(
		/DMO_WINDOWS=1
		/Zc:forScope
		/nologo
		)

#	if(MSVC80 OR MSVC90)
#		set(CMAKE_CXX_FLAGS_RELEASE
#			"${CMAKE_CXX_FLAGS_RELEASE} -D_SECURE_STL=0 -D_HAS_ITERATOR_DEBUGGING=0"
#			CACHE STRING "C++ compiler release options" FORCE)
#	endif (MSVC80 OR MSVC90)

	# Are we using the crummy Visual Studio KDU build workaround?
	option( VS_DISABLE_FATAL_WARNINGS "Turn off warnings as errors in VC++" FALSE )
	if( NOT VS_DISABLE_FATAL_WARNINGS )
		add_definitions( /WX )
	endif( NOT VS_DISABLE_FATAL_WARNINGS )
endif()

if( MO_MINGW32 )
	add_definitions(
		-DMO_MINGW32=1
		-D_WIN32_WINNT=0x0500
		-D_WIN32_IE=0x0500
		-D_REENTRANT
		-fexceptions
		-fno-math-errno
		-fno-strict-aliasing
		-fsigned-char
		)
endif()

if( MO_LINUX )
	add_definitions(
		-DMO_LINUX=1
		-DHAVE_LIBINTL_H=1
		-D_REENTRANT
		-fexceptions
		-fno-math-errno
		-fno-strict-aliasing
		-fsigned-char
		-pthread
		-Wno-deprecated
		-Wno-parentheses
		)
endif()

if( MO_DARWIN )
	add_definitions( -DMO_DARWIN=1 )
	set( CMAKE_CXX_LINK_FLAGS      "-Wl,-headerpad_max_install_names,-search_paths_first" )
	set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_CXX_LINK_FLAGS}"                              )
	set( CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} -mlong-branch"                     )
	set( CMAKE_C_FLAGS             "${CMAKE_C_FLAGS} -mlong-branch"                       )
	#
	# NOTE: it's critical that the optimization flag is put in front.
	#       and to have both CXX_FLAGS and C_FLAGS covered.
	set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O0 ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" )
	set( CMAKE_C_FLAGS_RELWITHDEBINFO   "-O0 ${CMAKE_C_FLAGS_RELWITHDEBINFO}"   )
endif()

if( MO_LINUX OR MO_DARWIN )
	set( GCC_WARNINGS "-Wall -Wno-sign-compare -Wno-trigraphs" )

	option( GCC_DISABLE_FATAL_WARNINGS "Set to allow warnings--otherwise they are considered errors." FALSE )
	if( NOT GCC_DISABLE_FATAL_WARNINGS )
		set( GCC_WARNINGS "${GCC_WARNINGS} -Werror" )
	endif (NOT GCC_DISABLE_FATAL_WARNINGS)

	set( GCC_CXX_WARNINGS "${GCC_WARNINGS} -Wno-reorder"           )
	set( CMAKE_C_FLAGS    "${GCC_WARNINGS} ${CMAKE_C_FLAGS}"       )
	set( CMAKE_CXX_FLAGS  "${GCC_CXX_WARNINGS} ${CMAKE_CXX_FLAGS}" )
endif()

# vim: ts=4 sw=4 noexpandtab
