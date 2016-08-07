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
#include "legacy_character.h"
#include "transaction.h"
#include "transactions/CharacterEntry.h"
#include "SingletonManager.h"
#include "version.h"
#include "mo/mo_props_xml.h"

#include <gtkmm.h>

using namespace Attribute;

namespace Application
{

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
	f_statsBagName				("STATS"						),
	f_charBagName				("CHARACTERS"					),
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
	// Default to d20 die
	//
	f_initDieStrings.push_back( "1d20" );

	// Get current default font for all widgets
	//
	const Glib::ustring font = Gtk::Settings::get_default()->property_gtk_font_name();
	f_combatantListFont = f_altCombatantListFont = font.c_str();
}


bool AppSettings::CheckVersion()
{
	const molib::moWCString version(turnwatcher_PACKAGE_VERSION);
	return version <= f_version;
}


void AppSettings::AddCharacter( Combatant::Character::Pointer ch, Transactions::TransactionGroup::Pointer group )
{
	Transactions::Transaction::Pointer tr( new Transactions::AddCharacterTransaction( ch ) );
	tr->doit();

	if( group )
	{
		group->addTransaction( tr );
	}
}


void AppSettings::SetLegacyCharacters( const Combatant::LegacyCharacter::List& lchars, Transactions::TransactionGroup::Pointer group )
{
	for( auto lch : lchars )
	{
		Combatant::Character::Pointer ch( new Combatant::Character );
		ch->Copy( lch );
		AddCharacter( ch, group );
	}
}


bool AppSettings::GetVersion( molib::moPropBagRef& bag, molib::moWCString& version )
{
	molib::moPropStringRef	versionProp( f_versionPropName );
	versionProp.Link( bag );

	bool has_version = false;
	//
	if( versionProp.HasProp() )
	{
		version = versionProp;
		has_version = true;
	}

	return has_version;
}


void AppSettings::LoadDieStrings( molib::moPropBagRef& mainBag )
{
	f_initDieStrings.clear();

	molib::moPropArrayRef	array("INITDIESTRINGS");
	array.Link( mainBag );
	if( !array.HasProp() )	array.NewProp();

	int count = array.CountIndexes();

	for( int idx = 0; idx < count; ++idx )
	{
		molib::moPropSPtr		prop_ptr = array.GetAtIndex( idx );
		molib::moPropStringRef	p_dieval( "DIEVAL" );
		p_dieval.NewProp();
		p_dieval.GetProperty()->Copy( *prop_ptr );
		molib::moWCString	dieval( p_dieval );
		f_initDieStrings.push_back( dieval );
	}
}


void AppSettings::SaveDieStrings( molib::moPropBagRef& mainBag )
{
	molib::moPropArrayRef	array("INITDIESTRINGS");
	array.NewProp();

	unsigned long idx = 0L;
	for( auto diestr : f_initDieStrings )
	{
		molib::moPropStringRef	dieval( "DIEVAL" );
		dieval.NewProp();
		dieval = diestr;
		array.Set( idx++, dieval.GetProperty() );
	}

	mainBag += array;
}


#if defined( DEMO_VERSION )

void AppSettings::LoadDemoCharacters()
{
	auto statMgr = Application::Manager::Instance().lock()->GetStatMgr().lock();
	const int	initId  = statMgr->initId();
	const int	spotId	= statMgr->spotId();
	const int	listenId= statMgr->listenId();
	const int	willId	= statMgr->willId();
	const int	levelId	= statMgr->levelId();

	Combatant::CharacterSPtr ch = new Combatant::Character();
	ch->name( "Errol" );
	ch->monster( false );
	ch->baseHP( 36 );
	ch->setMod( initId, 4 );
	ch->setMod( spotId, 2 );
	ch->setMod( listenId, 1 );
	ch->setMod( willId, 4 );
	ch->setMod( levelId, 5 );
	AddCharacter( ch, NULL );
	//
	ch = new Combatant::Character();
	ch->name( "Stubs" );
	ch->monster( false );
	ch->baseHP( 45 );
	ch->setMod( initId, 1 );
	ch->setMod( spotId, -1 );
	ch->setMod( listenId, -1 );
	ch->setMod( willId, 5 );
	ch->setMod( levelId, 5 );
#if 0
	Effects::EffectSPtr effect = new Effects::Effect;
	effect->name( "Bull Strength" );
	effect->description( "Gives a +4 to strength" );
	effect->totalRounds( 110 );
	ch->addEffect( effect );
#endif
	AddCharacter( ch, NULL );
	//
	ch = new Combatant::Character();
	ch->name( "Arial" );
	ch->monster( false );
	ch->baseHP( 24 );
	ch->setMod( initId, 1 );
	ch->setMod( spotId, 1 );
	ch->setMod( listenId, 1 );
	ch->setMod( willId, 4 );
	ch->setMod( levelId, 5 );
	AddCharacter( ch, NULL );
	//
	ch = new Combatant::Character();
	ch->name( "K the Knife" );
	ch->monster( false );
	ch->baseHP( 37 );
	ch->setMod( initId, 4 );
	ch->setMod( spotId, 4 );
	ch->setMod( listenId, 4 );
	ch->setMod( willId, 6 );
	ch->setMod( levelId, 5 );
	AddCharacter( ch, NULL );
	//
	ch = new Combatant::Character();
	ch->name( "Sinclair" );
	ch->monster( true );
	ch->baseHP( 35 );
	ch->setMod( initId, 4 );
	ch->setMod( spotId, 2 );
	ch->setMod( listenId, 2 );
	ch->setMod( willId, 8 );
	ch->setMod( levelId, 5 );
	AddCharacter( ch, NULL );
	//
	ch = new Combatant::Character();
	ch->name( "Dench" );
	ch->monster( true );
	ch->baseHP( 90 );
	ch->setMod( initId, 3 );
	ch->setMod( spotId, 1 );
	ch->setMod( listenId, 1 );
	ch->setMod( willId, 6 );
	ch->setMod( levelId, 5 );
	AddCharacter( ch, NULL );
}

#else


bool AppSettings::LoadCharacters( const molib::moWCString& filename )
{
	bool succeeded = false;
	//
	molib::moPropBagRef _charPropBag( "CHARACTERS" );
	const int ret_val = molib::moXMLLoadPropBag( filename, _charPropBag );
	//
	// 0 means file exists, -1 means not there (or error)
	//
	if( ret_val == 0 )
	{
		const molib::moWCString app_version = Version();
		molib::moWCString file_version;
		if( GetVersion( _charPropBag, file_version ) )
		{
			Transactions::TransactionGroupHelper helper( "Import New Characters" );
			LoadCharacters( _charPropBag, helper.GetGroup() );
			//
			succeeded = true;
		}
		else
		{
			LegacyApp lapp;
			//
			if( lapp.LoadCharacters( _charPropBag ) )
			{
				Combatant::LegacyCharacter::List charList;
				lapp.GetCharList( charList );
				Transactions::TransactionGroupHelper helper( "Import New Characters" );
				SetLegacyCharacters( charList, helper.GetGroup() );
				//
				succeeded = true;
			}
			else
			{
				// TODO: issue warning and maybe try to load...?
				succeeded = false;
			}
		}
	}
	
	return succeeded;
}


void AppSettings::SaveCharacters( const molib::moWCString& filename )
{
	molib::moPropBagRef _charPropBag( "CHARACTERS" );
	_charPropBag.NewProp();
	SaveCharacters( _charPropBag, true /*addVersion*/ );
	//
	molib::moXMLSavePropBag( filename, _charPropBag );
}


void AppSettings::LoadCharacters( molib::moPropBagRef& charBag, Transactions::TransactionGroup::Pointer group )
{
	auto charMgr = Application::Manager::Instance().lock()->GetCharacterMgr().lock();
	assert(charMgr);

	molib::moPropArrayRef	array("CHARACTERS");
	array.Link( charBag );
	if( !array.HasProp() )	array.NewProp();

	int count = array.CountIndexes();

	for( int idx = 0; idx < count; ++idx )
	{
		const int 			item_no  ( array.ItemNoAtIndex( idx ) );
		molib::moPropSPtr	prop_ptr ( array.Get( item_no ) );
		molib::moName		name	 ( prop_ptr->GetName()  );
		molib::moPropBagRef	propBag(name);
		propBag.NewProp();
		propBag.GetProperty()->Copy( *prop_ptr );

		Combatant::Character::Pointer ch( new Combatant::Character() );
		ch->Load( propBag );

		// Add "(copy)" in case of duplicates
		//
		Combatant::Character::Pointer dup = charMgr->FindDuplicates( ch );
		//
		while( dup )
		{
			ch->name( ch->name() + molib::moWCString(gettext(" (copy)"), -1, molib::mowc::MO_ENCODING_UTF8) );
			dup = charMgr->FindDuplicates( ch );
		}

		// Add character now that we are satisfied
		//
		AddCharacter( ch, group );
	}
}


void AppSettings::SaveCharacters( molib::moPropBagRef& charBag, const bool addVersion )
{
	molib::moPropArrayRef	array("CHARACTERS");
	array.NewProp();

	auto charMgr = Application::Manager::Instance().lock()->GetCharacterMgr().lock();
	assert(charMgr);
	const auto& chars = charMgr->GetCharacters();
	int array_index = 0;
	for( auto ch : chars )
	{
		molib::moPropBagRef propBag( "CHARACTER" );
		propBag.NewProp();
		ch->Save( propBag );
		array.Set( array_index++, propBag );
	}

	if( addVersion )
	{
		molib::moPropStringRef	versionProp( f_versionPropName );
		versionProp.NewProp();
		versionProp	 = f_version;
		charBag		+= versionProp;
	}

	charBag += array;
}

#endif // !DEMO_VERSION


bool AppSettings::Load( const molib::moWCString& fileName )
{
	auto statMgr = Application::Manager::Instance().lock()->GetStatMgr().lock();
	bool loaded;
	const molib::moWCString version(VERSION);

	// Load previous state
	//
	molib::moPropBagRef mainPropBag( f_mainBagName	);
	//
	const int ret_val = molib::moXMLLoadPropBag( fileName.c_str(), mainPropBag );

	// 0 means file exists, -1 means not there (or error)
	//
	if( ret_val == 0 )
	{
		molib::moPropBagRef		_statsBag	 				( f_statsBagName				);
		molib::moPropBagRef		_charBag					( f_charBagName					);
		molib::moPropIntRef		_toolbarPosProp				( f_toolbarPosPropName			);
		molib::moPropIntRef		_rollInitOnStartProp		( f_rollInitOnStartPropName		);
		molib::moPropIntRef		_manualInitiativeProp		( f_manualInitiativePropName	);
		molib::moPropIntRef		_windowXProp				( f_windowXPropName				);
		molib::moPropIntRef		_windowYProp				( f_windowYPropName				);
		molib::moPropIntRef		_windowHeightProp			( f_windowHeightPropName		);
		molib::moPropIntRef		_windowWidthProp			( f_windowWidthPropName			);
		molib::moPropIntRef		_hudXProp					( f_hudXPropName				);
		molib::moPropIntRef		_hudYProp					( f_hudYPropName				);
		molib::moPropIntRef		_hudHeightProp				( f_hudHeightPropName			);
		molib::moPropIntRef		_hudWidthProp				( f_hudWidthPropName			);
		molib::moPropStringRef	_currentFolderProp			( f_currentFolderPropName		);
		molib::moPropIntRef		_ultraInitProp				( f_ultraInitPropName			);
		molib::moPropIntRef		_showToolbarProp			( f_showToolbarPropName			);
		molib::moPropIntRef		_bleedOutDyingProp			( f_bleedOutDyingPropName		);
		molib::moPropIntRef		_skipDeadProp				( f_skipDeadPropName			);
		molib::moPropIntRef		_altDeathProp				( f_altDeathPropName			);
		molib::moPropIntRef		_notifyExpiredEffects		( f_notifyExpiredEffectsName	);
		molib::moPropIntRef		_panePositionProp			( f_panePositionPropName		);
		molib::moPropIntRef		_getDCProp					( f_getDCPropName				);
		molib::moPropIntRef		_lastDCProp					( f_lastDCPropName				);
		molib::moPropStringRef	_versionProp				( f_versionPropName				);
		molib::moPropIntRef		_showEffectsProp			( f_showEffectsPropName			);
		molib::moPropIntRef		_showInfoProp				( f_showInfoPropName			);
		molib::moPropIntRef		_showHUDProp				( f_showHUDPropName				);
		molib::moPropIntRef		_deathThresholdProp			( f_deathThresholdPropName		);
		molib::moPropBagRef		_initDieStringsProp			( f_initDieStringsPropName		);
		molib::moPropStringRef	_combatantListFontProp		( f_combatantListFontName		);
		molib::moPropStringRef	_altCombatantListFontProp	( f_altCombatantListFontName	);

		// Load application state
		//
		_toolbarPosProp		.Link(mainPropBag);
		if( _toolbarPosProp	.HasProp() )		{ f_toolbarPos		= _toolbarPosProp	; }
		//
		_currentFolderProp	.Link(mainPropBag);
		if( _currentFolderProp	.HasProp() )		{ f_currentFolder	= _currentFolderProp	; }
		//
		_windowXProp		.Link(mainPropBag);
		if( _windowXProp	.HasProp() )		{ f_windowX		= _windowXProp		; }
		//
		_windowYProp		.Link(mainPropBag);
		if( _windowYProp	.HasProp() )		{ f_windowY		= _windowYProp		; }
		//
		_windowHeightProp	.Link(mainPropBag);
		if( _windowHeightProp	.HasProp() )		{ f_windowHeight	= _windowHeightProp	; }
		//
		_windowWidthProp	.Link(mainPropBag);
		if( _windowWidthProp	.HasProp() )		{ f_windowWidth		= _windowWidthProp	; }
		//
		_hudXProp		.Link(mainPropBag);
		if( _hudXProp		.HasProp() )		{ f_hudX		= _hudXProp		; }
		//
		_hudYProp		.Link(mainPropBag);
		if( _hudYProp		.HasProp() )		{ f_hudY		= _hudYProp		; }
		//
		_hudHeightProp		.Link(mainPropBag);
		if( _hudHeightProp	.HasProp() )		{ f_hudHeight		= _hudHeightProp	; }
		//
		_hudWidthProp		.Link(mainPropBag);
		if( _hudWidthProp	.HasProp() )		{ f_hudWidth		= _hudWidthProp	; }
		//
		_ultraInitProp	 	.Link(mainPropBag);
		if( _ultraInitProp	.HasProp() )		{ f_ultraInit		= _ultraInitProp	; }
		//
		_bleedOutDyingProp 	.Link(mainPropBag);
		if( _bleedOutDyingProp	.HasProp() )		{ f_bleedOutDying	= _bleedOutDyingProp	; }
		//
		_skipDeadProp	 	.Link(mainPropBag);
		if( _skipDeadProp	.HasProp() )		{ f_skipDead		= _skipDeadProp		; }
		//
		_altDeathProp	 	.Link(mainPropBag);
		if( _altDeathProp	.HasProp() )		{ f_altDeath		= _altDeathProp		; }
		//
		_altDeathProp	 	.Link(mainPropBag);
		if( _altDeathProp	.HasProp() )		{ f_altDeath		= _altDeathProp		; }
		//
		_notifyExpiredEffects 	.Link(mainPropBag);
		if( _notifyExpiredEffects.HasProp() )		{ f_notifyExpiredEffects= _notifyExpiredEffects	; }
		//
		_showToolbarProp	.Link(mainPropBag);
		if( _showToolbarProp	.HasProp() )		{ f_showToolbar		= _showToolbarProp	; }
		//
		_rollInitOnStartProp	.Link(mainPropBag);
		if( _rollInitOnStartProp.HasProp() )		{ f_rollInitOnStart	= _rollInitOnStartProp	; }
		//
		_manualInitiativeProp	.Link(mainPropBag);
		if( _manualInitiativeProp.HasProp() )		{ f_manualInitiative	= _manualInitiativeProp	; }
		//
		_panePositionProp	.Link(mainPropBag);
		if( _panePositionProp	.HasProp() )		{ f_panePosition	= _panePositionProp	; }
		//
		_getDCProp		.Link(mainPropBag);
		if( _getDCProp		.HasProp() )		{ f_getDC		= _getDCProp		; }
		//
		_lastDCProp		.Link(mainPropBag);
		if( _lastDCProp		.HasProp() )		{ f_lastDC		= _lastDCProp		; }
		//
		_showEffectsProp	.Link(mainPropBag);
		if( _showEffectsProp	.HasProp() )		{ f_showEffects		= _showEffectsProp	; }
		//
		_showInfoProp		.Link(mainPropBag);
		if( _showInfoProp	.HasProp() )		{ f_showInfo		= _showInfoProp		; }
		//
		_showHUDProp		.Link(mainPropBag);
		if( _showHUDProp	.HasProp() )		{ f_showHUD		= _showHUDProp		; }
		//
		_deathThresholdProp	.Link(mainPropBag);
		if( _deathThresholdProp	.HasProp() )		{ f_deathThreshold	= _deathThresholdProp	; }
		//
		_combatantListFontProp	.Link(mainPropBag);
		if( _combatantListFontProp.HasProp() )		{ f_combatantListFont	= _combatantListFontProp; }
		//
		_altCombatantListFontProp.Link(mainPropBag);
		if( _altCombatantListFontProp.HasProp() )	{ f_altCombatantListFont= _altCombatantListFontProp; }
		//
		_initDieStringsProp	.Link(mainPropBag);
		if( _initDieStringsProp	.HasProp() )
		{
			LoadDieStrings( _initDieStringsProp );
		}

		// Handle version
		//
		_versionProp.Link( mainPropBag );
		if( _versionProp.HasProp() )
		{
			molib::moWCString conf_version = static_cast<molib::moWCString>(_versionProp );
			//
			const long	conf_period		 = conf_version.FindInString( "." );
			const int	int_conf_version = atoi( conf_version.Delete( conf_period, conf_period ).c_str() );
			//
			const long	period		= version.FindInString( "." );
			const int	int_version = atoi( version.Delete( period, period ).c_str() );
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
		f_version = version;

		// Load Stats
		//
		_statsBag.Link( mainPropBag );
		if( _statsBag.HasProp() )
		{
			statMgr->Load( _statsBag );
		}
		else
		{
			statMgr->AddDefaultColumns();
		}

#if !defined(DEMO_VERSION)
		// Load characters
		//
		_charBag.Link( mainPropBag );
		if( _charBag.HasProp() )
		{
			LoadCharacters( _charBag, NULL );
		}

		// Load initiative ordering
		//
		auto initMgr = Application::Manager::Instance().lock()->GetInitMgr().lock();
		initMgr->Clear();
		initMgr->Load( mainPropBag );
#endif

		loaded = true;
	}
	else
	{
		statMgr->AddDefaultColumns();
	}

#if defined(DEMO_VERSION)
	// Load demo characters and effects
	//
	LoadDemoCharacters();
	//
	auto initMgr = Application::Manager::Instance().lock()->GetInitMgr().lock();
	initMgr->Clear();
#endif

	return loaded;
}


void AppSettings::Save( const molib::moWCString& fileName )
{
	molib::moPropBagRef	mainPropBag		( f_mainBagName			);
	molib::moPropBagRef	_statsBag	 	( f_statsBagName		);
	molib::moPropBagRef	_charBag		( f_charBagName			);
	molib::moPropIntRef	_toolbarPosProp		( f_toolbarPosPropName		);
	molib::moPropIntRef	_rollInitOnStartProp	( f_rollInitOnStartPropName	);
	molib::moPropIntRef	_manualInitiativeProp	( f_manualInitiativePropName	);
	molib::moPropIntRef	_windowXProp		( f_windowXPropName		);
	molib::moPropIntRef	_windowYProp		( f_windowYPropName		);
	molib::moPropIntRef	_windowHeightProp	( f_windowHeightPropName	);
	molib::moPropIntRef	_windowWidthProp	( f_windowWidthPropName		);
	molib::moPropIntRef	_hudXProp		( f_hudXPropName		);
	molib::moPropIntRef	_hudYProp		( f_hudYPropName		);
	molib::moPropIntRef	_hudHeightProp		( f_hudHeightPropName		);
	molib::moPropIntRef	_hudWidthProp		( f_hudWidthPropName		);
	molib::moPropStringRef	_currentFolderProp	( f_currentFolderPropName	);
	molib::moPropIntRef	_ultraInitProp		( f_ultraInitPropName		);
	molib::moPropIntRef	_showToolbarProp	( f_showToolbarPropName		);
	molib::moPropIntRef	_bleedOutDyingProp	( f_bleedOutDyingPropName	);
	molib::moPropIntRef	_skipDeadProp		( f_skipDeadPropName		);
	molib::moPropIntRef	_altDeathProp		( f_altDeathPropName		);
	molib::moPropIntRef	_notifyExpiredEffects	( f_notifyExpiredEffectsName	);
	molib::moPropIntRef	_panePositionProp	( f_panePositionPropName	);
	molib::moPropIntRef	_getDCProp		( f_getDCPropName		);
	molib::moPropIntRef	_lastDCProp		( f_lastDCPropName		);
	molib::moPropStringRef	_versionProp		( f_versionPropName		);
	molib::moPropIntRef	_showEffectsProp	( f_showEffectsPropName		);
	molib::moPropIntRef	_showInfoProp		( f_showInfoPropName		);
	molib::moPropIntRef	_showHUDProp		( f_showHUDPropName		);
	molib::moPropIntRef	_deathThresholdProp	( f_deathThresholdPropName	);
	molib::moPropBagRef	_initDieStringsProp	( f_initDieStringsPropName	);
	molib::moPropStringRef	_combatantListFontProp	( f_combatantListFontName	);
	molib::moPropStringRef	_altCombatantListFontProp( f_altCombatantListFontName	);

	// Create propbag and populate it
	//
	_toolbarPosProp		.NewProp();	_toolbarPosProp		= f_toolbarPos		;
	_currentFolderProp	.NewProp(); 	_currentFolderProp	= f_currentFolder	;
	_windowXProp		.NewProp(); 	_windowXProp		= f_windowX		;
	_windowYProp		.NewProp(); 	_windowYProp		= f_windowY		;
	_windowHeightProp	.NewProp(); 	_windowHeightProp	= f_windowHeight	;
	_windowWidthProp	.NewProp(); 	_windowWidthProp	= f_windowWidth		;
	_hudXProp		.NewProp(); 	_hudXProp		= f_hudX		;
	_hudYProp		.NewProp(); 	_hudYProp		= f_hudY		;
	_hudHeightProp		.NewProp(); 	_hudHeightProp		= f_hudHeight		;
	_hudWidthProp		.NewProp(); 	_hudWidthProp		= f_hudWidth		;
	_ultraInitProp		.NewProp(); 	_ultraInitProp		= f_ultraInit		;
	_bleedOutDyingProp 	.NewProp(); 	_bleedOutDyingProp 	= f_bleedOutDying 	;
	_skipDeadProp	 	.NewProp(); 	_skipDeadProp	 	= f_skipDead	 	;
	_altDeathProp	 	.NewProp(); 	_altDeathProp	 	= f_altDeath	 	;
	_notifyExpiredEffects	.NewProp(); 	_notifyExpiredEffects	= f_notifyExpiredEffects;
	_showToolbarProp	.NewProp(); 	_showToolbarProp	= f_showToolbar		;
	_rollInitOnStartProp	.NewProp(); 	_rollInitOnStartProp	= f_rollInitOnStart	;
	_manualInitiativeProp	.NewProp(); 	_manualInitiativeProp	= f_manualInitiative	;
	_panePositionProp	.NewProp(); 	_panePositionProp	= f_panePosition	;
	_getDCProp		.NewProp(); 	_getDCProp		= f_getDC		;
	_lastDCProp		.NewProp(); 	_lastDCProp		= f_lastDC		;
	_versionProp		.NewProp(); 	_versionProp		= f_version		;
	_showEffectsProp	.NewProp(); 	_showEffectsProp	= f_showEffects		;
	_showInfoProp		.NewProp(); 	_showInfoProp		= f_showInfo		;
	_showHUDProp		.NewProp(); 	_showHUDProp		= f_showHUD		;
	_deathThresholdProp	.NewProp();	_deathThresholdProp	= f_deathThreshold	;
	_combatantListFontProp	.NewProp();	_combatantListFontProp	= f_combatantListFont	;
	_altCombatantListFontProp.NewProp();	_altCombatantListFontProp= f_altCombatantListFont;

	mainPropBag.NewProp();
	mainPropBag += _toolbarPosProp		;
	mainPropBag += _currentFolderProp	;
	mainPropBag += _windowXProp		;
	mainPropBag += _windowYProp		;
	mainPropBag += _windowHeightProp	;
	mainPropBag += _windowWidthProp	;
	mainPropBag += _hudXProp		;
	mainPropBag += _hudYProp		;
	mainPropBag += _hudHeightProp		;
	mainPropBag += _hudWidthProp		;
	mainPropBag += _ultraInitProp		;
	mainPropBag += _bleedOutDyingProp 	;
	mainPropBag += _skipDeadProp	 	;
	mainPropBag += _altDeathProp	 	;
	mainPropBag += _notifyExpiredEffects	;
	mainPropBag += _showToolbarProp	;
	mainPropBag += _rollInitOnStartProp	;
	mainPropBag += _manualInitiativeProp	;
	mainPropBag += _panePositionProp	;
	mainPropBag += _getDCProp		;
	mainPropBag += _lastDCProp		;
	mainPropBag += _versionProp		;
	mainPropBag += _showEffectsProp	;
	mainPropBag += _showInfoProp		;
	mainPropBag += _showHUDProp		;
	mainPropBag += _deathThresholdProp	;
	mainPropBag += _combatantListFontProp	;
	mainPropBag += _altCombatantListFontProp;

	// Save die faces
	//
	_initDieStringsProp.NewProp();
	SaveDieStrings( _initDieStringsProp );
	mainPropBag.Set( molib::moIndexOrName(_initDieStringsProp.GetName()), _initDieStringsProp );

	// Save Stats
	//
	_statsBag.NewProp();
	Application::Manager::Instance().lock()->GetStatMgr().lock()->Save( _statsBag );
	mainPropBag.Set( molib::moIndexOrName(_statsBag.GetName()), _statsBag );

#if !defined(DEMO_VERSION)
	// Save characters
	//
	_charBag.NewProp();
	SaveCharacters( _charBag );
	mainPropBag.Set( molib::moIndexOrName(_charBag.GetName()), _charBag );

	// Save initiative positions
	//
	Application::Manager::Instance().lock()->GetInitMgr().lock()->Save( mainPropBag );
#endif

	// Save file
	//
	molib::moXMLSavePropBag( fileName.c_str(), mainPropBag );
}

}
// namespace Application

// vim: ts=4 sw=4

