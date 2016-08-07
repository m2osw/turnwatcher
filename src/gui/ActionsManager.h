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

// Local
//
#include "common.h"
#include "character.h"
#include "motk/ActionManager.h"
#include "motk/UiBase.h"

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// STL
//
#include <memory>

namespace Actions
{


class Desensitizer
{
public:
	Desensitizer();
	~Desensitizer();
	
private:
	typedef std::map<molib::moWCString,bool> ActionStates;
	ActionStates m_actionStates;

	void PushAll();
	void PopAll();
	void DesensitizeAction( motk::ActionPtr action );
	void SensitizeAction( ActionStates::value_type pair );
};

class Manager
	: public motk::UIBase
	, public motk::ActionManager
{
public:
		typedef std::shared_ptr<Manager> Pointer;

		// Construction/destruction
		//
		Manager();
		~Manager();

		// Actions management
		//
		void						    CreateActions();	// Main menu/toolbar actions--do this first
		void						    ActivateAction( const molib::moWCString& name );
		bool							ToggleAction( const molib::moWCString& name );
		motk::ActionPtr					GetAction( const molib::moWCString& name );
		std::vector<motk::ActionPtr>	GetActions();
		void						    HandleSoftColumnChange();
		void						    SetSensitive( const bool sensitive = true );

		const molib::mo_name_t			f_startImage	;
		const molib::mo_name_t			f_endImage	;
		const molib::mo_name_t			f_nextImage	;
		const molib::mo_name_t			f_jumpInImage	;
		const molib::mo_name_t			f_delayImage	;
		const molib::mo_name_t			f_readyImage	;
		const molib::mo_name_t			f_damageImage	;
		const molib::mo_name_t			f_addImage 	;
		const molib::mo_name_t			f_editImage	;
		const molib::mo_name_t			f_deleteImage	;
		const molib::mo_name_t			f_d20InitImage  ;
		const molib::mo_name_t			f_d20SpotImage  ;
		const molib::mo_name_t			f_d20ListenImage;
		const molib::mo_name_t			f_d20WillImage  ;
		const molib::mo_name_t			f_websiteImage	;

		// UIBase stuff
		//
		motk::AccelGroupPtr				GetAccelGroup()				{ return UIBase::GetAccelGroup(); }

		// Widget instantiation
		//
		Gtk::Widget*					GetMenubar();
		Gtk::Widget*					GetMainToolbar();
		Gtk::Widget*					GetEffectsToolbar();
		Gtk::Widget*					GetHUDMenu();
		
		// Alter soft columns for menubar
		//
		void							AlterUI();

		// CharacterListUI
		//
		typedef std::vector<Combatant::Character::Pointer>	Characters;
		void OnSelectionChanged( Characters selection_list );

private:
		motk::MergeId					f_menubarId;
		motk::MergeId					f_mainToolbarId;
		motk::MergeId					f_startEndButtonId;
		motk::MergeId					f_softColumnsMenuId;
		motk::MergeId					f_softColumnsToolbarId;
		motk::MergeId					f_effectsToolbarId;
		motk::MergeId					f_hudMenubarId;

		typedef std::map<molib::mo_name_t,Manager::ButtonImageSPtr>	ButtonImages;
		ButtonImages					f_images;
		
		typedef std::vector<Glib::ustring>	StringList;
		StringList						f_softColumnsOrder;
		
		// Helper methods for dealing with altering the toolbars/menu
		//
		void							AlterSoftColumnsMenu();
		void							AlterSoftColumnsToolbar();
		void							AlterStopStartButtonToolbar();

		// Helper methods for handling soft columns
		//
		void							PurgeDeletedActions();
		void							AddSoftActions();
		Glib::ustring					GetSoftMenus( const bool toolitem );

		// motk::ActionManager
		//
		virtual void					OnButtonClicked( Glib::RefPtr<Gtk::Action>& action, const Glib::ustring& event );

		// Forbidden
		//
		//Manager( const Manager& ) {}
		Manager& operator =( const Manager& ) { return *this; }
};

}

// vim: ts=4 sw=4 noet syntax=cpp.doxygen

