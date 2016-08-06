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

project( turnwatcher )

option( DEMO_VERSION "Turn on demo-only mode" FALSE )
if( DEMO_VERSION )
	add_definitions( -DDEMO_VERSION )
	set( BIN_NAME turnwatcher-demo )
else()
	set( BIN_NAME turnwatcher )
endif()

configure_file( 
	"${PROJECT_SOURCE_DIR}/config/version.h.in"
	"${PROJECT_BINARY_DIR}/config/version.h"
	)	

set( DATADIR share/${BIN_NAME} )

set( VERSION "${turnwatcher_VERSION_MAJOR}.${turnwatcher_VERSION_MINOR}.${turnwatcher_VERSION_PATCH}" )
add_definitions(
		-DVERSION=\"${VERSION}\"
		-DDATADIR=\"${DATADIR}\"
		-DPACKAGE_NAME=\"${turnwatcher_PACKAGE_NAME}\"
		-DPACKAGE_VERSION=\"${VERSION}\"
		)

configure_file(
	"${PROJECT_SOURCE_DIR}/config/system.h.in"
	"${PROJECT_BINARY_DIR}/config/system.h"
	)	

include( 00-Common )
find_package( SigCpp )
find_package( Qt4 4.8.1 REQUIRED QtCore QtGui )
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )
set( CMAKE_AUTOMOC TRUE )

include_directories(
		${CMAKE_SOURCE_DIR}
		${PROJECT_SOURCE_DIR}
		${PROJECT_BINARY_DIR}
		${PROJECT_BINARY_DIR}/config
		${PROJECT_SOURCE_DIR}/base
		${PROJECT_SOURCE_DIR}/ui
		${PROJECT_SOURCE_DIR}/transactions
		${molib_SOURCE_DIR}/include
		${QT_INCLUDE_DIRS}
		${SIGC_INCLUDE_DIRS}
		)

set( BASE_HEADER_FILES
    base/character.h
    base/common.h
    base/effect.h
    base/LegacyApp.h
    base/LegacyCharacter.h
    base/stat.h
    base/ActionsManager.h
    base/AppSettings.h
    base/CharacterColumns.h
    base/CharacterManager.h
    base/CharacterModel.h
	base/DuplicateResolver.h
	base/DuplicateRoll.h
    base/InitiativeManager.h
    base/ManagerBase.h
    base/SoftColumns.h
    base/StatManager.h
    base/transaction.h
   )

set( GUI_HEADER_FILES
	ui/AboutWindow.h
	ui/CharacterView.h
	ui/DamageWindow.h
	ui/DefineInitWindow.h
	ui/DuplicateWindow.h
	ui/EditWindow.h
	ui/EffectEditor.h
	ui/EffectsBook.h
	ui/EffectsEditor.h
	ui/EffectsList.h
	ui/ExpiredEffectsWindow.h
	ui/HUDWindow.h
	ui/InfoBox.h
	ui/InfoWindow.h
	ui/InitWindow.h
	ui/InputWindow.h
	ui/JumpInWindow.h
	ui/MainWindow.h
	ui/RoundsHelper.h
	ui/SaveWindow.h
	ui/SettingsWindow.h
    ui/Splash.h
    ui/StatEditor.h
	ui/StatusBox.h
	ui/StatWindow.h
	ui/TableConnections.h
	)

set( TRANSACTION_HEADER_FILES
    transactions/transactions.h
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
    config/build_version.h
    ${BASE_HEADER_FILES}
	${GUI_HEADER_FILES}
	${TRANSACTION_HEADER_FILES}
	)

set( BASE_SOURCE_FILES
    base/character.cpp
    base/common.cpp
    base/effect.cpp
    base/LegacyApp.cpp
    base/LegacyCharacter.cpp
    base/stat.cpp
    base/ActionsManager.cpp
    base/AppSettings.cpp
    base/CharacterColumns.cpp
    base/CharacterManager.cpp
    base/CharacterModel.cpp
	base/DuplicateResolver.cpp
	base/DuplicateRoll.cpp
    base/InitiativeManager.cpp
    base/SoftColumns.cpp
    base/StatManager.cpp
	base/transaction.cpp
   )

set( GUI_SOURCE_FILES
	ui/AboutWindow.cpp
	ui/CharacterView.cpp
	ui/DamageWindow.cpp
	ui/DefineInitWindow.cpp
	ui/DuplicateWindow.cpp
	ui/EditWindow.cpp
	ui/EffectEditor.cpp
	ui/EffectsBook.cpp
	ui/EffectsEditor.cpp
	ui/EffectsList.cpp
	ui/ExpiredEffectsWindow.cpp
	ui/HUDWindow.cpp
	ui/InfoBox.cpp
	ui/InfoWindow.cpp
	ui/InitWindow.cpp
	ui/InputWindow.cpp
	ui/JumpInWindow.cpp
	ui/MainWindow.cpp
	ui/RoundsHelper.cpp
	ui/SettingsWindow.cpp
    ui/Splash.cpp
    ui/StatEditor.cpp
	ui/StatusBox.cpp
	ui/StatWindow.cpp
	)

set( TRANSACTION_SOURCE_FILES
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

##
## Qt4 Stuff
##
QT4_WRAP_UI( UIC_FILES
	ui/MainWindow.ui
	)
QT4_ADD_RESOURCES( RES_FILES
    res/resources.qrc
    )

set( SOURCE_FILES
    TurnWatcher.cpp
	${RES_FILES}
	${UIC_FILES}
	${BASE_SOURCE_FILES}
    ${GUI_SOURCE_FILES}
	${TRANSACTION_SOURCE_FILES}
   )

if( DEMO_VERSION )
	list( APPEND HEADER_FILES NagWindow.h )
	list( APPEND SOURCE_FILES NagWindow.cpp )
endif()

if( MO_WINDOWS )
	enable_language( RC )
	set( CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> /fo <OBJECT> <SOURCE>" )
	set( RESOURCE_FILES
            win32/resources.rc
			pixmaps/turnwatcher.ico
			)
	source_group( "Resource Files" FILES ${RESOURCE_FILES} )
    list( APPEND HEADER_FILES win32/resources.h )
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
	${QT_LIBRARIES}
	${SIGC_LIBRARIES}
	molib
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

set( IMAGE_DIR ${CMAKE_SOURCE_DIR}/images )

install( FILES
			${IMAGE_DIR}/turnwatcher-splash.png
			${IMAGE_DIR}/turnwatcher-about.png
		DESTINATION
			${DATADIR}/images
		)

install( FILES
			pixmaps/turnwatcher.png
		DESTINATION
			share/pixmaps
		)

set( DOCFILES )
foreach( subdir en fr es images styles )
	install(	DIRECTORY ${CMAKE_SOURCE_DIR}/doc/${subdir}
				DESTINATION ${DATADIR}/doc/${subdir}
				PATTERN ".hg" EXCLUDE
				PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ
			)
endforeach()

if( MO_LINUX )
	configure_file(
		"${CMAKE_SOURCE_DIR}/turnwatcher.desktop.in"
		"${PROJECT_BINARY_DIR}/${BIN_NAME}.desktop"
		)	
	install( FILES ${PROJECT_BINARY_DIR}/${BIN_NAME}.desktop DESTINATION share/applications )
endif()

# vim: ts=4 sw=4 noexpandtab
