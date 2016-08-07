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




#pragma once

// LOCAL
//
#include "effect.h"
#include "character.h"
#include "legacy_app.h"
#include "transaction.h"

// STL
//
#include <memory>

namespace Application
{

class AppSettings
{
public:
	typedef std::shared_ptr<AppSettings> Pointer;

	AppSettings();
	~AppSettings() {}

	bool CheckVersion();

#if defined( DEMO_VERSION )
	void LoadDemoCharacters();
#endif

	void AddEffectToPool( Effects::Effect::Pointer effect );

	bool GetVersion( molib::moPropBagRef& bag, molib::moWCString& version );

#ifdef DARWIN
	// With Mac OS/X, we don't add the menu to the vbox--we just
	// add the toolbar only, so 0 is the top
	//
	enum ToolbarPosition { TBP_TOP = 0, TBP_BOTTOM = 1 };
#else
	enum ToolbarPosition { TBP_TOP = 1, TBP_BOTTOM = 2 };
#endif

	int					ToolBarPos()							{ return f_toolbarPos			; }
	molib::moWCString	UserPath()								{ return f_currentFolder		; }

	int					X()										{ return f_windowX				; }
	int					Y()										{ return f_windowY				; }
	int					Width()									{ return f_windowWidth			; }
	int					Height()								{ return f_windowHeight			; }

	int					HUDX()									{ return f_hudX					; }
	int					HUDY()									{ return f_hudY					; }
	int					HUDWidth()								{ return f_hudWidth				; }
	int					HUDHeight()								{ return f_hudHeight			; }

	bool				UltraInit()								{ return f_ultraInit			; }
	bool				BleedOutDying()							{ return f_bleedOutDying		; }
	bool				SkipDead()								{ return f_skipDead				; }
	bool				AlternateDeathRule()					{ return f_altDeath				; }
	bool				ShowToolbar()							{ return f_showToolbar			; }
	bool				InitOnStart()							{ return f_rollInitOnStart		; }
	bool				ManualInit()							{ return f_manualInitiative		; }
	int					PanePosition()							{ return f_panePosition			; }
	bool				GetDC()									{ return f_getDC				; }
	int					LastDC()								{ return f_lastDC				; }
	molib::moWCString	Version()								{ return f_version				; }
	bool				ShowEffects()							{ return f_showEffects			; }
	bool				ShowInfo()								{ return f_showInfo				; }
	bool				ShowHUD()								{ return f_showHUD				; }
	bool				Modified() const						{ return f_modified				; }
	molib::moWCString	StatusMsg() const						{ return f_statusMsg			; }
	int					DeathThreshold() const					{ return f_deathThreshold		; }
#ifdef WANT_EFFECTS
	bool				NotifyExpiredEffects() const			{ return (bool) f_notifyExpiredEffects; }
#endif	
	molib::moWCString	CombatantListFont() const				{ return f_combatantListFont	; }
	molib::moWCString	AltCombatantListFont() const			{ return f_altCombatantListFont	; }

	typedef std::vector<molib::moWCString> Strings;
	Strings				InitDieStrings() const					{ return f_initDieStrings		; }

	void ToolBarPos (int			val )						{ f_toolbarPos				= val; 	}
	void UserPath   (const molib::moWCString& val )				{ f_currentFolder			= val; 	}

	void X					(int	val )						{ f_windowX					= val;	}
	void Y					(int	val )						{ f_windowY					= val;	}
	void Width      		(int	val )						{ f_windowWidth				= val; 	}
	void Height     		(int	val )						{ f_windowHeight			= val; 	}

	void HUDX				(int	val )						{ f_hudX					= val;	}
	void HUDY				(int	val )						{ f_hudY					= val;	}
	void HUDWidth   		(int	val )						{ f_hudWidth				= val; 	}
	void HUDHeight  		(int	val )						{ f_hudHeight				= val; 	}

	void UltraInit  		(bool	val )						{ f_ultraInit				= val; 	}
	void BleedOutDying		(bool	val )						{ f_bleedOutDying			= val; 	}
	void SkipDead   		(bool	val )						{ f_skipDead				= val; 	}
	void AlternateDeathRule	(bool	val )						{ f_altDeath				= val;	}
	void ShowToolbar		(bool	val )						{ f_showToolbar				= val; 	}
	void InitOnStart		(bool	val )						{ f_rollInitOnStart	 		= val; 	}
	void ManualInit 		(bool	val )						{ f_manualInitiative		= val; 	}
	void PanePosition		(int	val )						{ f_panePosition			= val;	}
	void GetDC      		(bool	val )						{ f_getDC					= val; 	}
	void LastDC     		(int	val )						{ f_lastDC					= val; 	}
	void ShowEffects		(bool	val )						{ f_showEffects				= val; 	}
	void ShowInfo   		(bool	val )						{ f_showInfo				= val; 	}
	void ShowHUD			(bool	val )						{ f_showHUD					= val;	}
	void Modified			(bool	val )						{ f_modified				= val; 	}
	void StatusMsg			(const molib::moWCString& val )		{ f_statusMsg				= val; 	}
	void DeathThreshold		(const int		val )				{ f_deathThreshold			= val;	}
#ifdef WANT_EFFECTS
	void NotifyExpiredEffects( bool val )						{ f_notifyExpiredEffects	= val; }
#endif
	void CombatantListFont( const molib::moWCString& val )		{ f_combatantListFont		= val; }
	void AltCombatantListFont( const molib::moWCString& val )	{ f_altCombatantListFont	= val; }

	void InitDieStrings( const Strings& val )					{ f_initDieStrings		= val; }

	void SetLegacyCharacters( const Combatant::LegacyCharacter::List& lchars, Transactions::TransactionGroup::Pointer group );

	typedef sigc::signal<void> VoidSignal;
	VoidSignal			signal_changed() 						{ return f_changedSignal; }

#if !defined( DEMO_VERSION )
	bool LoadCharacters( const molib::moWCString& filename );
	void SaveCharacters( const molib::moWCString& filename );
	//
	void LoadCharacters( molib::moPropBagRef& charBag, Transactions::TransactionGroup::Pointer group );
	void SaveCharacters( molib::moPropBagRef& charBag, const bool addVersion = false );
#endif

	bool Load( const molib::moWCString& fileName );
	void Save( const molib::moWCString& fileName );
		
private:
	VoidSignal			f_changedSignal			;

	bool				f_modified				;
	molib::moWCString	f_statusMsg				;

	int					f_toolbarPos			;
	molib::moWCString	f_currentFolder			;

	int					f_windowX				;
	int					f_windowY				;
	int					f_windowHeight			;
	int					f_windowWidth			;

	int					f_hudX					;
	int					f_hudY					;
	int					f_hudHeight				;
	int					f_hudWidth				;

	bool				f_ultraInit	 			;
	bool				f_bleedOutDying			;
	bool				f_skipDead	 			;
	bool				f_altDeath				;
	bool				f_notifyExpiredEffects	;
	bool				f_showToolbar			;
	bool				f_rollInitOnStart		;
	bool				f_manualInitiative		;
	int					f_panePosition			;
	int					f_getDC					;
	int					f_lastDC				;
	molib::moWCString	f_version				;
	bool				f_showEffects			;
	bool				f_showInfo				;
	bool				f_showHUD				;
	int					f_deathThreshold		;
	Strings				f_initDieStrings		;
	molib::moWCString	f_combatantListFont		;
	molib::moWCString	f_altCombatantListFont	;

	// Names
	//
	molib::moName		f_mainBagName				;
	molib::moName		f_statsBagName				;
	molib::moName		f_charBagName				;
	molib::moName		f_toolbarPosPropName		;
	molib::moName		f_rollInitOnStartPropName	;
	molib::moName		f_manualInitiativePropName	;

	molib::moName		f_windowXPropName			;
	molib::moName		f_windowYPropName			;
	molib::moName		f_windowHeightPropName		;
	molib::moName		f_windowWidthPropName		;

	molib::moName		f_hudXPropName				;
	molib::moName		f_hudYPropName				;
	molib::moName		f_hudHeightPropName			;
	molib::moName		f_hudWidthPropName			;

	molib::moName		f_currentFolderPropName		;
	molib::moName		f_ultraInitPropName			;
	molib::moName		f_showToolbarPropName		;
	molib::moName		f_bleedOutDyingPropName		;
	molib::moName		f_skipDeadPropName			;
	molib::moName		f_altDeathPropName			;
	molib::moName		f_notifyExpiredEffectsName	;
	molib::moName		f_panePositionPropName		;
	molib::moName		f_getDCPropName				;
	molib::moName		f_lastDCPropName			;
	molib::moName		f_versionPropName			;
	molib::moName		f_showEffectsPropName		;
	molib::moName		f_showInfoPropName			;
	molib::moName		f_showHUDPropName			;
	molib::moName		f_deathThresholdPropName	;
	molib::moName		f_initDieStringsPropName	;
	molib::moName		f_combatantListFontName		;
	molib::moName		f_altCombatantListFontName	;

	void AddCharacter( Combatant::Character::Pointer ch, Transactions::TransactionGroup::Pointer group );
	void LoadDieStrings( molib::moPropBagRef& mainBag );
	void SaveDieStrings( molib::moPropBagRef& mainBag );
};

}
// namespace Application


// vim: ts=4 sw=4

