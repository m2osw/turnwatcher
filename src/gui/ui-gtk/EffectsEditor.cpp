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

// LOCAL
//
#include "EffectsEditor.h"
#include "EffectEditor.h"

#include <math.h>
#include <stack>

using namespace molib;
using namespace Effects;

namespace UI
{

EffectsEditor::EffectsEditor()
		: f_addButton   ( gettext("Add") )
		, f_deleteButton( gettext("Delete") )
		, f_editButton  ( gettext("Edit") )
{
	FillMainBox();
	AddTooltips();
	show_all_children();
}


EffectsEditor::~EffectsEditor()
{
}


void EffectsEditor::AddEffects( Combatant::Character::pointer_t ch )
{
	f_treeView.AddEffects( ch );
}


void EffectsEditor::GetEffects( Combatant::Character::pointer_t ch )
{
	f_treeView.GetEffects( ch );
}


void EffectsEditor::ClearEffects()
{
	f_treeView.ClearEffects();
}


void EffectsEditor::AddTooltips()
{
	// TODO: This does not work!
	//
	f_tooltips.set_tip( f_addButton   , gettext("Add a new effect") );
	f_tooltips.set_tip( f_deleteButton, gettext("Delete selected effect") );
	f_tooltips.set_tip( f_editButton  , gettext("Edit selected effect") );
	f_tooltips.enable();
}


void EffectsEditor::FillMainBox()
{
	Gtk::VBox* vbox = this;
	Gtk::HBox* hbox = 0;
	Gtk::Frame* frame = 0;

	frame = Gtk::manage(new Gtk::Frame(gettext("Effects")));

	f_scroller.add( f_treeView );
	frame->add( f_scroller );
	f_scroller.set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
	f_scroller.set_shadow_type( Gtk::SHADOW_IN  );
	f_scroller.set_border_width( 1 );

	hbox = Gtk::manage( new Gtk::HBox );
	hbox->pack_start( *frame, true, true );
	vbox->pack_start( *hbox, true, true );

	f_buttons.pack_start( f_addButton   , true, true );
	f_buttons.pack_start( f_deleteButton, true, true );
	f_buttons.pack_start( f_editButton  , true, true );
	vbox->pack_start( f_buttons, false, false );

	f_deleteButton.set_sensitive( false );
	f_editButton.set_sensitive( false );

	f_treeView.signal_changed().connect( sigc::mem_fun( *this, &EffectsEditor::OnSelected ) );

	f_addButton   .signal_clicked().connect( sigc::bind( sigc::mem_fun( *this, &EffectsEditor::OnEditButton   ), true ) );
	f_deleteButton.signal_clicked().connect( sigc::mem_fun( *this, &EffectsEditor::OnDeleteButton ) );
	f_editButton  .signal_clicked().connect( sigc::bind( sigc::mem_fun( *this, &EffectsEditor::OnEditButton   ), false ) );

	show_all_children();
}


void EffectsEditor::OnEditButton( const bool add )
{
	EffectEditor	editor( add? gettext("Add New Effect"): gettext("Edit Effect") );

	Effect::pointer_t	effect;
	QString	row_name;

	if( add )
	{
		effect.reset( new Effect );
	}
	else
	{
		effect = f_treeView.GetFirstSelected();
		row_name = effect->name();
	}

	editor.SetEffect( effect );

	while( true )
	{
		const int response_id = editor.run();

		if( response_id == Gtk::RESPONSE_OK )
		{
			bool dups = false;

			if( add )
			{
				dups = f_treeView.FindDuplicates( effect );
			}
			else
			{
				if( row_name != effect->name() )
				{
					dups = f_treeView.FindDuplicates( effect );
				}
			}

			if( dups )
			{
				Gtk::MessageDialog dlg
					( gettext("Duplicate effect name--use another one!")
					, false
					, Gtk::MESSAGE_ERROR
					, Gtk::BUTTONS_OK
					, true
					);
				dlg.run();
			}
			else
			{
				if( add )
				{
					f_treeView.AddEntry( effect );
				}
				else
				{
					f_treeView.UpdateEffect( effect );
				}
				break;
			}
		}
		else
		{
			break;
		}
	}
}


void EffectsEditor::OnDeleteButton()
{
	f_treeView.DeleteSelected();
}


void EffectsEditor::OnSelected()
{
	const int selected_count = f_treeView.SelectedCount();
	f_deleteButton.set_sensitive( selected_count  > 0 );
	f_editButton  .set_sensitive( selected_count == 1 );
}


}
// namespace UI


// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen

