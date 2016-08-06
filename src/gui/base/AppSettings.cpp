//===============================================================================
// Copyright (c) 2005-2013 by Made to Order Software Corporation
// 
// All Rights Reserved.
// 
// The source code in this file ("Source Code") is provided by Made to Order Software Corporation
// to you under the terms of the GNU General Public License, version 2.0
// ("GPL").  Terms of the GPL can be found in doc/GPL-license.txt in this distribution.
// 
// By copying, modifying or distributing this software, you acknowledge
// that you have read and understood your obligations described above,
// and agree to abide by those obligations.
// 
// ALL SOURCE CODE IN THIS DISTRIBUTION IS PROVIDED "AS IS." THE AUTHOR MAKES NO
// WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
// COMPLETENESS OR PERFORMANCE.
//===============================================================================


// LOCAL
//
#include "common.h"
#include "LegacyCharacter.h"
#include "transaction.h"
#include "transactions/CharacterEntry.h"
#include "version.h"
#include "mo/mo_props_xml.h"

#include <gtkmm.h>

using namespace Attribute;
using namespace molib;

namespace Application
{

AppSettings::private_pointer_t AppSettings::f_instance;

AppSettings::AppSettings() :
	f_modified					(false),
	f_toolbarPos				(TBP_TOP),
	f_rollInitOnStart			(true),
	f_manualInitiative			(true),
	f_windowX					(-1),
	f_windowY					(-1),
	f_windowWidth 				(800),
	f_windowHeight				(600),
	f_hudX						(-1),
	f_hudY						(-1),
	f_hudWidth 					(640),
	f_hudHeight					(480),
	f_ultraInit					(false),
	f_showToolbar 				(true),
	f_bleedOutDying				(true),
	f_skipDead 					(true),
	f_altDeath					(false),
	f_notifyExpiredEffects		(true),
	f_panePosition 				(-1),
	f_getDC						(true),
	f_lastDC 					(-1),
	f_version					(turnwatcher_PACKAGE_VERSION),
	f_showEffects				(true),
	f_showInfo					(true),
	f_showHUD					(false),
	f_deathThreshold			(-10),		
	f_mainBagName				("TURNWATCHER"),
	//f_statsBagName				("STATS"						),
	//f_charBagName				("CHARACTERS"					),
	f_toolbarPosPropName		("TOOLBAR_POS"					),
	f_rollInitOnStartPropName	("ROLL_INIT_ON_START"			),
	f_manualInitiativePropName	("MANUAL_INIT"					),
	f_windowXPropName			("WINDOW_X"						),
	f_windowYPropName			("WINDOW_Y"						),
	f_windowHeightPropName		("WINDOW_HEIGHT"				),
	f_windowWidthPropName		("WINDOW_WIDTH"					),
	f_hudXPropName				("HUD_X"						),
	f_hudYPropName				("HUD_Y"						),
	f_hudHeightPropName			("HUD_HEIGHT"					),
	f_hudWidthPropName			("HUD_WIDTH"					),
	f_currentFolderPropName		("CURRENT_FOLDER"				),
	f_ultraInitPropName			("ULTRA_INITIATIVE"				),
	f_showToolbarPropName		("SHOW_TOOLBAR"					),
	f_bleedOutDyingPropName		("BLEED_OUT_DYING"				),
	f_skipDeadPropName			("SKIP_DEAD"					),
	f_altDeathPropName			("ALTERNATE_DEATH_RULE"			),
	f_notifyExpiredEffectsName	("NOTIFY_EXPIRED_EFFECTS"		),
	f_panePositionPropName		("PANE_POSITION"	   			),
	f_getDCPropName				("GET_DC"		   				),
	f_lastDCPropName			("LAST_DC"		   				),
	f_versionPropName			("VERSION"		   				),
	f_showEffectsPropName		("SHOW_EFFECTS"	   				),
	f_showInfoPropName			("SHOW_INFO"					),
	f_showHUDPropName			("SHOW_HUD"						),
	f_deathThresholdPropName	("DEATH_THRESHOLD"				),
	f_initDieStringsPropName	("INIT_DIE_STRINGS"				),
	f_combatantListFontName		("COMBATANT_LIST_FONT_NAME"		),
	f_altCombatantListFontName	("ALT_COMBATANT_LIST_FONT_NAME"	)
{
	if( !Load() )
	{
		// Default to d20 die
		//
		f_initDieStrings.push_back( "1d20" );

		// Get current default font for all widgets
		//
		const Glib::ustring font = Gtk::Settings::get_default()->property_gtk_font_name();
		f_combatantListFont = f_altCombatantListFont = font;
	}
}


AppSettings::~AppSettings()
{
}


AppSettings::pointer_t AppSettings::Instance()
{
	if( !f_instance )
	{
		f_instance.reset( new AppSettings );
	}
	return f_instance;
}


void AppSettings::Release()
{
	f_instance.reset();
}


bool AppSettings::CheckVersion()
{
	const Glib::ustring version(turnwatcher_PACKAGE_VERSION);
	return version <= f_version;
}


bool AppSettings::GetVersion( moPropBagRef& bag, Glib::ustring& version )
{
	moPropStringRef	versionProp( f_versionPropName );
	versionProp.Link( bag );

	bool has_version = false;
	//
	if( versionProp.HasProp() )
	{
		version = static_cast<moWCString>(versionProp).c_str();
		has_version = true;
	}

	return has_version;
}


void AppSettings::AddVersion( moPropBagRef& bag )
{
	molib::moPropStringRef	versionProp( f_versionPropName );
	versionProp.NewProp();
    versionProp = static_cast<moWCString>(f_version.c_str());
	bag        += versionProp;
}


void AppSettings::LoadDieStrings( moPropBagRef& mainBag )
{
	f_initDieStrings.clear();

	moPropArrayRef	array("INITDIESTRINGS");
	array.Link( mainBag );
	if( !array.HasProp() )	array.NewProp();

	int count = array.CountIndexes();

	for( int idx = 0; idx < count; ++idx )
	{
		moPropSPtr		prop_ptr = array.GetAtIndex( idx );
		moPropStringRef	dieval( "DIEVAL" );
		dieval.NewProp();
		dieval.GetProperty()->Copy( *prop_ptr );
		f_initDieStrings.push_back( static_cast<moWCString>(dieval).c_str() );
	}
}


void AppSettings::SaveDieStrings( moPropBagRef& mainBag )
{
	moPropArrayRef	array("INITDIESTRINGS");
	array.NewProp();

	unsigned long idx = 0L;
	for( auto diestr : f_initDieStrings )
	{
		moPropStringRef	dieval( "DIEVAL" );
		dieval.NewProp();
		dieval = diestr.c_str();
		array.Set( idx++, dieval.GetProperty() );
	}

	mainBag += array;
}


bool AppSettings::Load()
{
	const moWCString version(VERSION);

	// Load previous state
	//
	moPropBagRef mainPropBag( f_mainBagName	);
	if( !Common::LoadBagFromFile( "turnwatcher.conf", mainPropBag ) ) return false;

	moPropIntRef    toolbarPosProp           ( f_toolbarPosPropName       );
	moPropIntRef    rollInitOnStartProp      ( f_rollInitOnStartPropName  );
	moPropIntRef    manualInitiativeProp     ( f_manualInitiativePropName );
	moPropIntRef    windowXProp              ( f_windowXPropName          );
	moPropIntRef    windowYProp              ( f_windowYPropName          );
	moPropIntRef    windowHeightProp         ( f_windowHeightPropName     );
	moPropIntRef    windowWidthProp          ( f_windowWidthPropName      );
	moPropIntRef    hudXProp                 ( f_hudXPropName             );
	moPropIntRef    hudYProp                 ( f_hudYPropName             );
	moPropIntRef    hudHeightProp            ( f_hudHeightPropName        );
	moPropIntRef    hudWidthProp             ( f_hudWidthPropName         );
	moPropStringRef currentFolderProp        ( f_currentFolderPropName    );
	moPropIntRef    ultraInitProp            ( f_ultraInitPropName        );
	moPropIntRef    showToolbarProp          ( f_showToolbarPropName      );
	moPropIntRef    bleedOutDyingProp        ( f_bleedOutDyingPropName    );
	moPropIntRef    skipDeadProp             ( f_skipDeadPropName         );
	moPropIntRef    altDeathProp             ( f_altDeathPropName         );
	moPropIntRef    notifyExpiredEffects     ( f_notifyExpiredEffectsName );
	moPropIntRef    panePositionProp         ( f_panePositionPropName     );
	moPropIntRef    getDCProp                ( f_getDCPropName            );
	moPropIntRef    lastDCProp               ( f_lastDCPropName           );
	moPropStringRef versionProp              ( f_versionPropName          );
	moPropIntRef    showEffectsProp          ( f_showEffectsPropName      );
	moPropIntRef    showInfoProp             ( f_showInfoPropName         );
	moPropIntRef    showHUDProp              ( f_showHUDPropName          );
	moPropIntRef    deathThresholdProp       ( f_deathThresholdPropName   );
	moPropBagRef    initDieStringsProp       ( f_initDieStringsPropName   );
	moPropStringRef combatantListFontProp    ( f_combatantListFontName    );
	moPropStringRef altCombatantListFontProp ( f_altCombatantListFontName );

	// Load application state
	//
	toolbarPosProp.Link(mainPropBag);           if( toolbarPosProp.HasProp()           ) { f_toolbarPos           = toolbarPosProp;                                            }
	currentFolderProp.Link(mainPropBag);        if( currentFolderProp.HasProp()        ) { f_currentFolder        = static_cast<moWCString>(currentFolderProp).c_str();        }
	windowXProp.Link(mainPropBag);              if( windowXProp.HasProp()              ) { f_windowX              = windowXProp;                                               }
	windowYProp.Link(mainPropBag);              if( windowYProp.HasProp()              ) { f_windowY              = windowYProp;                                               }
	windowHeightProp.Link(mainPropBag);         if( windowHeightProp.HasProp()         ) { f_windowHeight         = windowHeightProp;                                          }
	windowWidthProp.Link(mainPropBag);          if( windowWidthProp.HasProp()          ) { f_windowWidth          = windowWidthProp;                                           }
	hudXProp.Link(mainPropBag);                 if( hudXProp.HasProp()                 ) { f_hudX                 = hudXProp;                                                  }
	hudYProp.Link(mainPropBag);                 if( hudYProp.HasProp()                 ) { f_hudY                 = hudYProp;                                                  }
	hudHeightProp.Link(mainPropBag);            if( hudHeightProp.HasProp()            ) { f_hudHeight            = hudHeightProp;                                             }
	hudWidthProp.Link(mainPropBag);             if( hudWidthProp.HasProp()             ) { f_hudWidth             = hudWidthProp;                                              }
	ultraInitProp.Link(mainPropBag);            if( ultraInitProp.HasProp()            ) { f_ultraInit            = ultraInitProp;                                             }
	bleedOutDyingProp.Link(mainPropBag);        if( bleedOutDyingProp.HasProp()        ) { f_bleedOutDying        = bleedOutDyingProp;                                         }
	skipDeadProp.Link(mainPropBag);             if( skipDeadProp.HasProp()             ) { f_skipDead             = skipDeadProp;                                              }
	altDeathProp.Link(mainPropBag);             if( altDeathProp.HasProp()             ) { f_altDeath             = altDeathProp;                                              }
	notifyExpiredEffects.Link(mainPropBag);     if( notifyExpiredEffects.HasProp()     ) { f_notifyExpiredEffects = notifyExpiredEffects;                                      }
	showToolbarProp.Link(mainPropBag);          if( showToolbarProp.HasProp()          ) { f_showToolbar          = showToolbarProp;                                           }
	rollInitOnStartProp.Link(mainPropBag);      if( rollInitOnStartProp.HasProp()      ) { f_rollInitOnStart      = rollInitOnStartProp;                                       }
	manualInitiativeProp.Link(mainPropBag);     if( manualInitiativeProp.HasProp()     ) { f_manualInitiative     = manualInitiativeProp;                                      }
	panePositionProp.Link(mainPropBag);         if( panePositionProp.HasProp()         ) { f_panePosition         = panePositionProp;                                          }
	getDCProp.Link(mainPropBag);                if( getDCProp.HasProp()                ) { f_getDC                = getDCProp;                                                 }
	lastDCProp.Link(mainPropBag);               if( lastDCProp.HasProp()               ) { f_lastDC               = lastDCProp;                                                }
	showEffectsProp.Link(mainPropBag);          if( showEffectsProp.HasProp()          ) { f_showEffects          = showEffectsProp;                                           }
	showInfoProp.Link(mainPropBag);             if( showInfoProp.HasProp()             ) { f_showInfo             = showInfoProp;                                              }
	showHUDProp.Link(mainPropBag);              if( showHUDProp.HasProp()              ) { f_showHUD              = showHUDProp;                                               }
	deathThresholdProp.Link(mainPropBag);       if( deathThresholdProp.HasProp()       ) { f_deathThreshold       = deathThresholdProp;                                        }
	combatantListFontProp.Link(mainPropBag);    if( combatantListFontProp.HasProp()    ) { f_combatantListFont    = static_cast<moWCString>(combatantListFontProp).c_str();    }
	altCombatantListFontProp.Link(mainPropBag); if( altCombatantListFontProp.HasProp() ) { f_altCombatantListFont = static_cast<moWCString>(altCombatantListFontProp).c_str(); }
	initDieStringsProp.Link(mainPropBag);       if( initDieStringsProp.HasProp()       ) { LoadDieStrings( initDieStringsProp ); }

	// Handle version
	//
	versionProp.Link( mainPropBag );
	if( versionProp.HasProp() )
	{
		const moWCString conf_version( versionProp );
		//
		const long	conf_period		 = conf_version.FindInString( "." );
		const int	int_conf_version = atoi( conf_version.Delete( conf_period, conf_period ).c_str() );
		//
		const long	period		= version.FindInString( "." );
		const int	int_version = atoi( version.Delete( period, period ).c_str() );
		//
		if( int_conf_version > int_version )
		{
			// TODO: Handle version change...
			//
			//f_version = version;
			// We won't try to load something from a higher version
			//
			return false;
		}
	}
	//
	f_version = version.c_str();

	return true;
}


bool AppSettings::Save()
{
	moPropBagRef    mainPropBag              ( f_mainBagName              );
	moPropIntRef    toolbarPosProp           ( f_toolbarPosPropName       );
	moPropIntRef    rollInitOnStartProp      ( f_rollInitOnStartPropName  );
	moPropIntRef    manualInitiativeProp     ( f_manualInitiativePropName );
	moPropIntRef    windowXProp              ( f_windowXPropName          );
	moPropIntRef    windowYProp              ( f_windowYPropName          );
	moPropIntRef    windowHeightProp         ( f_windowHeightPropName     );
	moPropIntRef    windowWidthProp          ( f_windowWidthPropName      );
	moPropIntRef    hudXProp                 ( f_hudXPropName             );
	moPropIntRef    hudYProp                 ( f_hudYPropName             );
	moPropIntRef    hudHeightProp            ( f_hudHeightPropName        );
	moPropIntRef    hudWidthProp             ( f_hudWidthPropName         );
	moPropStringRef currentFolderProp        ( f_currentFolderPropName    );
	moPropIntRef    ultraInitProp            ( f_ultraInitPropName        );
	moPropIntRef    showToolbarProp          ( f_showToolbarPropName      );
	moPropIntRef    bleedOutDyingProp        ( f_bleedOutDyingPropName    );
	moPropIntRef    skipDeadProp             ( f_skipDeadPropName         );
	moPropIntRef    altDeathProp             ( f_altDeathPropName         );
	moPropIntRef    notifyExpiredEffects     ( f_notifyExpiredEffectsName );
	moPropIntRef    panePositionProp         ( f_panePositionPropName     );
	moPropIntRef    getDCProp                ( f_getDCPropName            );
	moPropIntRef    lastDCProp               ( f_lastDCPropName           );
	moPropStringRef versionProp              ( f_versionPropName          );
	moPropIntRef    showEffectsProp          ( f_showEffectsPropName      );
	moPropIntRef    showInfoProp             ( f_showInfoPropName         );
	moPropIntRef    showHUDProp              ( f_showHUDPropName          );
	moPropIntRef    deathThresholdProp       ( f_deathThresholdPropName   );
	moPropBagRef    initDieStringsProp       ( f_initDieStringsPropName   );
	moPropStringRef combatantListFontProp    ( f_combatantListFontName    );
	moPropStringRef altCombatantListFontProp ( f_altCombatantListFontName );

	// Create propbag and populate it
	//
	toolbarPosProp           .NewProp(); toolbarPosProp           = f_toolbarPos                   ;
	currentFolderProp        .NewProp(); currentFolderProp        = f_currentFolder.c_str()        ;
	windowXProp              .NewProp(); windowXProp              = f_windowX                      ;
	windowYProp              .NewProp(); windowYProp              = f_windowY                      ;
	windowHeightProp         .NewProp(); windowHeightProp         = f_windowHeight                 ;
	windowWidthProp          .NewProp(); windowWidthProp          = f_windowWidth                  ;
	hudXProp                 .NewProp(); hudXProp                 = f_hudX                         ;
	hudYProp                 .NewProp(); hudYProp                 = f_hudY                         ;
	hudHeightProp            .NewProp(); hudHeightProp            = f_hudHeight                    ;
	hudWidthProp             .NewProp(); hudWidthProp             = f_hudWidth                     ;
	ultraInitProp            .NewProp(); ultraInitProp            = f_ultraInit                    ;
	bleedOutDyingProp        .NewProp(); bleedOutDyingProp        = f_bleedOutDying                ;
	skipDeadProp             .NewProp(); skipDeadProp             = f_skipDead                     ;
	altDeathProp             .NewProp(); altDeathProp             = f_altDeath                     ;
	notifyExpiredEffects     .NewProp(); notifyExpiredEffects     = f_notifyExpiredEffects         ;
	showToolbarProp          .NewProp(); showToolbarProp          = f_showToolbar                  ;
	rollInitOnStartProp      .NewProp(); rollInitOnStartProp      = f_rollInitOnStart              ;
	manualInitiativeProp     .NewProp(); manualInitiativeProp     = f_manualInitiative             ;
	panePositionProp         .NewProp(); panePositionProp         = f_panePosition                 ;
	getDCProp                .NewProp(); getDCProp                = f_getDC                        ;
	lastDCProp               .NewProp(); lastDCProp               = f_lastDC                       ;
	versionProp              .NewProp(); versionProp              = f_version.c_str()              ;
	showEffectsProp          .NewProp(); showEffectsProp          = f_showEffects                  ;
	showInfoProp             .NewProp(); showInfoProp             = f_showInfo                     ;
	showHUDProp              .NewProp(); showHUDProp              = f_showHUD                      ;
	deathThresholdProp       .NewProp(); deathThresholdProp       = f_deathThreshold               ;
	combatantListFontProp    .NewProp(); combatantListFontProp    = f_combatantListFont.c_str()    ;
	altCombatantListFontProp .NewProp(); altCombatantListFontProp = f_altCombatantListFont.c_str() ;

	mainPropBag.NewProp();
	mainPropBag += toolbarPosProp           ;
	mainPropBag += currentFolderProp        ;
	mainPropBag += windowXProp              ;
	mainPropBag += windowYProp              ;
	mainPropBag += windowHeightProp         ;
	mainPropBag += windowWidthProp          ;
	mainPropBag += hudXProp                 ;
	mainPropBag += hudYProp                 ;
	mainPropBag += hudHeightProp            ;
	mainPropBag += hudWidthProp             ;
	mainPropBag += ultraInitProp            ;
	mainPropBag += bleedOutDyingProp        ;
	mainPropBag += skipDeadProp             ;
	mainPropBag += altDeathProp             ;
	mainPropBag += notifyExpiredEffects     ;
	mainPropBag += showToolbarProp          ;
	mainPropBag += rollInitOnStartProp      ;
	mainPropBag += manualInitiativeProp     ;
	mainPropBag += panePositionProp         ;
	mainPropBag += getDCProp                ;
	mainPropBag += lastDCProp               ;
	mainPropBag += versionProp              ;
	mainPropBag += showEffectsProp          ;
	mainPropBag += showInfoProp             ;
	mainPropBag += showHUDProp              ;
	mainPropBag += deathThresholdProp       ;
	mainPropBag += combatantListFontProp    ;
	mainPropBag += altCombatantListFontProp ;

	// Save die faces
	//
	initDieStringsProp.NewProp();
	SaveDieStrings( initDieStringsProp );
	mainPropBag.Set( moIndexOrName(initDieStringsProp.GetName()), initDieStringsProp );

	// Save file
	//
	return Common::SaveBagToFile( "turnwatcher.conf", mainPropBag );
}

}
// namespace Application

// vim: ts=4 sw=4

