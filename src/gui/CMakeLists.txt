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

project( turnwatcher VERSION 1.5.2 )

option( DEMO_VERSION "Turn on demo-only mode" FALSE )
if( DEMO_VERSION )
	add_definitions( -DDEMO_VERSION )
	set( BIN_NAME turnwatcher-demo )
else()
	set( BIN_NAME turnwatcher )
endif()

configure_file( 
	"${PROJECT_SOURCE_DIR}/version.h.in"
	"${PROJECT_BINARY_DIR}/version.h"
	)	

set( DATADIR share/${BIN_NAME} )

set( VERSION "${turnwatcher_VERSION_MAJOR}.${turnwatcher_VERSION_MINOR}.${turnwatcher_VERSION_PATCH}" )
add_definitions(
		-DOLD_UI
		-DVERSION=\"${VERSION}\"
		-DDATADIR=\"${DATADIR}\"
		-DPACKAGE_NAME=\"${turnwatcher_PACKAGE_NAME}\"
		-DPACKAGE_VERSION=\"${VERSION}\"
		)

configure_file(
	"${PROJECT_SOURCE_DIR}/system.h.in"
	"${PROJECT_BINARY_DIR}/system.h"
	)	

include( 00-Common )
include( sigc      )
#include( boost     )
include( gtkmm     )
#include( qt4       )
find_package( Freetype  )

include_directories(
		${CMAKE_SOURCE_DIR}
		${PROJECT_SOURCE_DIR}
		${PROJECT_BINARY_DIR}
		${molib_SOURCE_DIR}/include
		${motk_SOURCE_DIR}
		${motk_SOURCE_DIR}/../
		${FREETYPE_INCLUDE_DIRS}
		${GTKMM_INCLUDE_DIRS}
		${SIGC_INCLUDE_DIRS}
		)

set( BASE_HEADER_FILES
	character.h
	common.h
	effect.h
	resources.h
	splash.h
	stat.h
	sync-menu.h
	SingletonManager.h
	ActionsManager.h
	CharacterManager.h
	InitiativeManager.h
	StatManager.h
   )

set( GUI_HEADER_FILES
	AbilityEditor.h
	AboutWindow.h
	AppSettings.h
	build_version.h
	CharacterColumns.h
	CharacterListUI.h
	DamageWindow.h
	DefineInitWindow.h
	DuplicateResolver.h
	DuplicateRoll.h
	DuplicateWindow.h
	EditWindow.h
	EffectEditor.h
	EffectsBook.h
	EffectsEditor.h
	EffectsList.h
	ExpiredEffectsWindow.h
	HUDWindow.h
	InitWindow.h
	InputWindow.h
	JumpInWindow.h
	legacy_app.h
	legacy_character.h
	MainWindow.h
	NagWindow.h
	SaveWindow.h
	SettingsWindow.h
	SoftColumns.h
	StatEditor.h
	StatusBox.h
	StatWindow.h
	TableConnections.h
	)

set( TRANSACTION_HEADER_FILES
	transaction.h
	transactions.h
	transactions/BleedOutTransaction.h
	transactions/CharacterEntry.h
	transactions/DamageTransaction.h
	transactions/DelayTransaction.h
	transactions/EffectHandler.h
	transactions/ForceInitiative.h
	transactions/InitiativeBase.h
	transactions/MoveCharacterInInitiative.h
	transactions/MoveSelectedCharacters.h
	transactions/NextInitTransaction.h
	transactions/ResolveInitiativeDuplicates.h
	transactions/RollTransaction.h
	transactions/SetDefaultSort.h
	transactions/SortTransaction.h
	transactions/StabilizeTransaction.h
	transactions/UITransactionBase.h
	transactions/UltraInitTransaction.h
   )

set( HEADER_FILES
	${BASE_HEADER_FILES}
	${GUI_HEADER_FILES}
	${TRANSACTION_HEADER_FILES}
	)

set( BASE_SOURCE_FILES
	character.cpp
	common.cpp
	effect.cpp
	splash.cpp
	stat.cpp
	ActionsManager.cpp
	SingletonManager.cpp
	CharacterManager.cpp
	InitiativeManager.cpp
	StatManager.cpp
   )

set( GUI_SOURCE_FILES
	AbilityEditor.cpp
	AboutWindow.cpp
	AppSettings.cpp
	CharacterColumns.cpp
	CharacterListUI.cpp
	DamageWindow.cpp
	DefineInitWindow.cpp
	DuplicateResolver.cpp
	DuplicateRoll.cpp
	DuplicateWindow.cpp
	EditWindow.cpp
	EffectEditor.cpp
	EffectsBook.cpp
	EffectsEditor.cpp
	EffectsList.cpp
	ExpiredEffectsWindow.cpp
	HUDWindow.cpp
	InitWindow.cpp
	InputWindow.cpp
	JumpInWindow.cpp
	legacy_app.cpp
	legacy_character.cpp
	MainWindow.cpp
	NagWindow.cpp
	SettingsWindow.cpp
	SoftColumns.cpp
	StatEditor.cpp
	StatusBox.cpp
	StatWindow.cpp
	TurnWatcher.cpp
	)

set( TRANSACTION_SOURCE_FILES
	transaction.cpp
	transactions/BleedOutTransaction.cpp
	transactions/CharacterEntry.cpp
	transactions/DamageTransaction.cpp
	transactions/DelayTransaction.cpp
	transactions/EffectHandler.cpp
	transactions/ForceInitiative.cpp
	transactions/InitiativeBase.cpp
	transactions/MoveCharacterInInitiative.cpp
	transactions/MoveSelectedCharacters.cpp
	transactions/NextInitTransaction.cpp
	transactions/ResolveInitiativeDuplicates.cpp
	transactions/RollTransaction.cpp
	transactions/SetDefaultSort.cpp
	transactions/SortTransaction.cpp
	transactions/StabilizeTransaction.cpp
	transactions/UITransactionBase.cpp
	)

set( SOURCE_FILES
	${BASE_SOURCE_FILES}
	${GUI_SOURCE_FILES}
	${TRANSACTION_SOURCE_FILES}
   )

if( MO_WINDOWS )
	enable_language( RC )
	set( CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> /fo <OBJECT> <SOURCE>" )
	set( RESOURCE_FILES
			resources.rc
			pixmaps/helmet.ico
			)
	source_group( "Resource Files" FILES ${RESOURCE_FILES} )
	list( APPEND HEADER_FILES resources.h )
	list( APPEND SOURCE_FILES ${RESOURCE_FILES} )
endif()

set( GETTEXT_MSGFMT_EXECUTABLE msgfmt )
set( LOCALEDIR ${CMAKE_BINARY_DIR}/src/po )

set( LANGUAGES es fr )
foreach( LANG ${LANGUAGES} )
	set( LC_MSGDIR ${LOCALEDIR}/${LANG}/LC_MESSAGES )
	add_custom_command(
		OUTPUT ${LC_MSGDIR}
		COMMAND ${CMAKE_COMMAND} -E make_directory 
				${LC_MSGDIR}
		COMMENT "mo-update [${LANG}]: Creating locale directory."
	)

	add_custom_command(
		OUTPUT ${LC_MSGDIR}/turnwatcher.mo
		COMMAND ${GETTEXT_MSGFMT_EXECUTABLE}
				-o ${LC_MSGDIR}/turnwatcher.mo ${LANG}.po
		DEPENDS 
			${LC_MSGDIR}
			${PROJECT_SOURCE_DIR}/po/${LANG}.po
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/po
		COMMENT "mo-update [${LANG}]: Creating mo file."
	)

	list( APPEND SOURCE_FILES ${LC_MSGDIR}/turnwatcher.mo )

	install(FILES
				${LC_MSGDIR}/turnwatcher.mo
			DESTINATION
				share/locale/${LANG}/LC_MESSAGES/${BIN_NAME}.mo
			)
endforeach()

list( APPEND SOURCE_FILES ${HEADER_FILES} ${CMAKE_FILES} )

# Make sure the linker can find the library once it is built.
link_directories(
	${ARCH_PREBUILT_DIRS}
	${SIGC_LIBRARY_DIRS}
	${PROJECT_SOURCE_DIR}
	)

add_executable( ${PROJECT_NAME} WIN32 ${SOURCE_FILES} )

if( MO_LINUX )
	set_target_properties(	${PROJECT_NAME}
							PROPERTIES
							INSTALL_RPATH ".;${CMAKE_INSTALL_PREFIX}/lib"
							BUILD_WITH_INSTALL_RPATH TRUE
							)
endif()

if( DEMO_VERSION )
	set_target_properties( ${PROJECT_NAME}
							PROPERTIES
							RUNTIME_OUTPUT_NAME
							"${PROJECT_NAME}-demo"
						)
endif()


target_link_libraries(
	${PROJECT_NAME}
	${FREEYPE_LIBRARIES}
	${GTKMM_LIBRARIES}
	${SIGC_LIBRARIES}
	${Boost_LIBRARIES}
	molib
	motk
	) 

#if (MO_WINDOWS)
#	set_target_properties(
#		${PROJECT_NAME}
#		PROPERTIES
#        LINK_FLAGS "/debug /SUBSYSTEM:WINDOWS"
#        LINK_FLAGS_DEBUG "/NODEFAULTLIB:MSVCRTD.lib;MSVCPRTD.lib"
#        LINK_FLAGS_RELEASE "/NODEFAULTLIB:MSVCRT.lib;MSVCPRT.lib"
#		)
#endif()



############################################################
## Installation

install( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )

set( IMAGE_DIR   ${CMAKE_SOURCE_DIR}/dev/images      )

install( FILES
			${IMAGE_DIR}/turnwatcher-splash.png
			${IMAGE_DIR}/turnwatcher-about.png
		DESTINATION
			${DATADIR}/images
		)

install( FILES
			pixmaps/turnwatcher.png
		DESTINATION
			share/pixmaps/${BIN_NAME}.png
		)

set( DOCFILES )
foreach( subdir en fr es images styles )
	install(	DIRECTORY ${CMAKE_SOURCE_DIR}/doc/${subdir}
				DESTINATION ${DATADIR}/doc/${subdir}
				PATTERN ".svn" EXCLUDE
				PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ
			)
endforeach()

if( MO_LINUX )
	configure_file(
		"${CMAKE_SOURCE_DIR}/dev/linux/turnwatcher.desktop.in"
		"${PROJECT_BINARY_DIR}/${BIN_NAME}.desktop"
		)	
	install( FILES ${PROJECT_BINARY_DIR}/${BIN_NAME}.desktop DESTINATION share/applications/kde )
	install( FILES ${CMAKE_SOURCE_DIR}/dev/linux/turnwatcher.menu DESTINATION share/menu RENAME ${BIN_NAME} )
endif()

# vim: ts=4 sw=4 noexpandtab
