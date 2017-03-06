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
#include "EffectsList.h"

#include <math.h>
#include <stack>

using namespace Effects;

namespace UI
{

EffectsList::EffectsList()
{
	FillMainBox();
	AddTooltips();
	set_enable_search( false );
	show_all_children();
}


EffectsList::~EffectsList()
{
}


void EffectsList::AddTooltips()
{
	// TODO: This does not work!
	//
	f_tooltips.enable();
}


void EffectsList::FillMainBox()
{
	f_store = Gtk::ListStore::create( f_columns );
	assert(f_store);
	set_model( f_store );

	append_column( gettext("Name"),		f_columns.f_name	);
	append_column( gettext("Total Rounds"),	f_columns.f_totalRounds	);
	append_column( gettext("Rounds Left"),	f_columns.f_roundsLeft	);
	append_column( gettext("Type"),		f_columns.f_type	);

	f_selection = get_selection();
	f_selection->set_mode( Gtk::SELECTION_MULTIPLE );
	f_selectionConnection = f_selection->signal_changed().connect( sigc::mem_fun( *this, &EffectsList::OnSelected ) );

	show_all_children();
}


void EffectsList::UpdateRow( Gtk::TreeModel::Row& row, const Effect::pointer_t effect )
{
	row[f_columns.f_name]		= effect->name().c_str();
	row[f_columns.f_totalRounds]	= effect->totalRounds();
	row[f_columns.f_roundsLeft]	= effect->totalRounds() - effect->roundsUsed();
	//
	QString typeName;
	switch( effect->type() )
	{
		case Effect::Ability:	typeName = gettext("Ability");		break;
		case Effect::Spell:	typeName = gettext("Spell");		break;
		default:		typeName = gettext("Other");		break;
	}
	//
	row[f_columns.f_type] = typeName;
	row[f_columns.f_effect] = effect;
}


void EffectsList::AddEntry( const Effect::pointer_t effect )
{
	assert(effect);
	if( !FindDuplicates( effect ) )
	{
		assert(f_store);
		Gtk::TreeModel::Row row( *(f_store->append()) );
		UpdateRow( row, effect );
	}
}


bool EffectsList::FindDuplicates( const Effect::pointer_t effect )
{
	assert(effect);
	assert(f_store);
	bool duplicate_found = false;
	for( auto row : f_store->children() )
	{
		Effect::pointer_t	row_effect = row[f_columns.f_effect];
		assert(row_effect);
		if( row_effect->name() == effect->name() )
		{
			duplicate_found = true;
			break;
		}
	}
	return duplicate_found;
}


void EffectsList::OnSelected()
{
    f_changedSignal.emit();
}


int EffectsList::SelectedCount() const
{
	SelectedPaths selected = f_selection->get_selected_rows();
	return (int) selected.size();
}


void EffectsList::AddEffects( Combatant::Character::pointer_t ch )
{
	ClearEffects();

	Effect::list_t	effect_list;
	ch->getEffects(effect_list);

	for( auto effect : effect_list )
	{
		assert(effect);
		AddEntry( effect );
	}
}


void EffectsList::GetEffects( Combatant::Character::pointer_t ch )
{
	assert(ch);
	assert(f_store);
	Effect::list_t effect_list;
	for( auto row : f_store->children() )
	{
		Effect::pointer_t	effect( row[f_columns.f_effect] );
		effect_list.push_back( effect );
	}

	ch->setEffects( effect_list );
}


void EffectsList::UpdateEffect( Effects::Effect::pointer_t effect_to_find )
{
	assert(effect_to_find);
	assert(f_store);
	for( auto row : f_store->children() )
	{
		Effect::pointer_t effect( row[f_columns.f_effect] );
		assert(effect);
		if( effect->name() == effect_to_find->name() )
		{
			UpdateRow( row, effect );
		}
	}
}


void EffectsList::ClearEffects()
{
	assert(f_store);
	f_store->clear();
}


Effects::Effect::pointer_t EffectsList::GetFirstSelected()
{
	assert(f_store);
	SelectedPaths            selected ( f_selection->get_selected_rows() );
	Gtk::TreeModel::Path     path     ( *(selected.begin())              );
	Gtk::TreeModel::Row      row      ( *(f_store->get_iter(path))       );
	Effects::Effect::pointer_t effect   ( row[f_columns.f_effect]          );
	assert(effect);
	return effect;
}


void EffectsList::DeleteSelected()
{
	assert(f_selection);
	assert(f_store);
	//
	f_selectionConnection.disconnect();

	typedef std::stack<Gtk::TreeModel::iterator> Iterators;
	Iterators to_delete;
	//
	for( auto path : f_selection->get_selected_rows() )
	{
		auto list_model_iter( f_store->get_iter( path ) );
		to_delete.push( list_model_iter );
	}
	//
	while( to_delete.size() > 0 )
	{
		Gtk::TreeModel::iterator iter( to_delete.top() );
		f_store->erase( iter );
		to_delete.pop();
	}
}


}
// namespace UI


// vim: ts=8 sw=8

