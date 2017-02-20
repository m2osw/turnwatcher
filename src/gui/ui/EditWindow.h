//===============================================================================
// Copyright (c) 2005-2017 by Made to Order Software Corporation
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

// STL
//
#include <map>

// MOLIB
//
#include "mo/mo_name.h"

// MOTK
//
#include "motk/LabelTable.h"

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// LOCAL
//
#include "common.h"
#include "character.h"
#include "TableConnections.h"
// At this time, the demo includes everything...
//#if !defined(DEMO_VERSION) && !defined(OLD_UI)
#if !defined(OLD_UI)
#include "effect.h"
#include "EffectsEditor.h"
#endif
#include "ManagerBase.h"

namespace UI
{

#define RESPONSE_EFFECTS 100

class EditWindow
	: public Gtk::Dialog
	, private Application::ManagerBase
{
public:
	EditWindow( const Glib::ustring& title, Gtk::Window& parent);
	~EditWindow();

	void	GetData( Combatant::Character::pointer_t ch );
	void	SetData( Combatant::Character::pointer_t ch );
	void	Clear()	{ f_char = 0; }

	typedef sigc::signal<void>	CharacterSignal;
	CharacterSignal	signal_changed() { return f_changedSignal; }
	
	int		GetFocusEntry() const		{ return f_focusEntry;	}
	void	SetFocusEntry( int val )	{ f_focusEntry = val;	}
	
private:
	typedef std::vector<Gtk::Widget*> WidgetList;

	CharacterSignal				  f_changedSignal;
	motk::LabelTable			  f_baseTable;
#if defined(OLD_UI)
	motk::LabelTable			  f_combatTable;
	motk::LabelTable			  f_statTable;
	bool						  f_showing;
#else
	motk::LabelTable              f_statTable;
	motk::LabelTable              f_savesModTable;
	motk::LabelTable              f_skillsModTable;
	Gtk::Notebook                 f_notebook;
	Gtk::TextView                 f_notesEntry;
	Glib::RefPtr<Gtk::TextBuffer> f_textBuffer;
	EffectsEditor                 f_effectsEditor;
#endif
	WidgetList                    f_entryWidgets;
	int                           f_focusEntry;
	Combatant::Character::pointer_t f_char;

#if defined(OLD_UI)
	typedef std::map<int,Attribute::Stat::pointer_t> StatMap;
	StatMap				f_statMap;
#else
	typedef std::vector<Attribute::Stat::pointer_t> StatList;
	StatList			f_abilityList;
	StatList			f_skillList;
	StatList			f_saveList;
#endif

	TableConnections	f_tableConnections;
	AdjConnections		f_adjConnections;
	sigc::connection	f_charConnection;

	Gtk::Tooltips		f_tooltips;

	void 		InitBaseTable();
#if defined(OLD_UI)
	void		InitCombatTable();
	void		InitStatTable();
	void		AddStatEntry( const char* name, const molib::mo_name_t id );
	void		AddStatNotes( const char* name, const molib::mo_name_t id );
	void		GetStats();
#else
	void		GetStats( StatList& list, Attribute::Stat::Type type );
	void		AddSavesTable ();
	void		AddSkillsTable();
	void 		InitModifierTable();
	void		AddStatEntry( const char* name, const int id );
	void 		InitStatTable();
#endif
	void 		FillMainBox();
	void		ClearTable( motk::LabelTable& table );
	void		ClearMainBox();
#if !defined(OLD_UI)
	void		GetTypeValues( motk::LabelTable& table, StatList& list );
	void		SetTypeValues( motk::LabelTable& table, StatList& list );
#endif
	void		UpdateDlgFromChar();
	void		UpdateCharFromDlg();
	void		UpdateDialog();

	// Gtk::Widget
	//
	virtual	void	on_show();
	virtual void	on_hide();
	virtual bool	on_key_press_event( GdkEventKey* event );

	// LabelTable
	//
	void OnSetFocus( motk::LabelTable* table );
	void OnToggled();

	// StatManager
	//
	void OnReloadTables();
};

}
// namespace UI


// vim: ts=4 sw=4 syntax=cpp.doxygen

