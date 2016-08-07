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
#include "StatEditor.h"
#include "SingletonManager.h"
#include "StatManager.h"

namespace UI
{

StatEditor::StatEditor()
{
	set_rules_hint();
	set_headers_visible( true );
	set_headers_clickable( true );
	set_enable_search( false );
		
	get_selection()->set_mode( Gtk::SELECTION_MULTIPLE );
	
	f_accelKeyStore = Gtk::ListStore::create( f_accelKeyColumns );
	assert(f_accelKeyStore);
	//
	f_dieFacesStore = Gtk::ListStore::create( f_dieFacesColumns  );
	assert(f_dieFacesStore);

	// Append all columns to the treeview
	//
	append_column( "Order",	f_columns.f_order	);
	append_column_editable( "Name",	f_columns.f_name	);
	AddDieFacesColumn();
	AddAccelKeyColumn();
	append_column_editable( "Show on Toolbar",    			f_columns.f_showOnToolbar );
	append_column_editable( "Show on Player HUD", 			f_columns.f_showOnHUD );
	append_column_editable( "Show in Monster Stat on HUD",  f_columns.f_showMonsterOnHUD );
	
	// Order column
	//
	int col_num = 0;
	Gtk::TreeView::Column* col = get_column( col_num++ );
	col->set_sort_column( f_columns.f_order );
	Gtk::CellRendererText* rend = dynamic_cast<Gtk::CellRendererText*>(*(col->get_cell_renderers().begin()));
	rend->signal_edited().connect( sigc::mem_fun( *this, &StatEditor::OnOrderEdited ) );
	//
	// Name column
	//
	col = f_nameColumn = get_column( col_num++ );
	col->set_sort_column( f_columns.f_name );
	rend = dynamic_cast<Gtk::CellRendererText*>(*(col->get_cell_renderers().begin()));
	rend->signal_edited().connect( sigc::mem_fun( *this, &StatEditor::OnNameEdited ) );
	//
	// Die Faces
	//
	col = get_column( col_num++ );
	col->set_sort_column( f_columns.f_dieFaces );
	rend = dynamic_cast<Gtk::CellRendererText*>(*(col->get_cell_renderers().begin()));
	col->add_attribute( rend->property_sensitive(), f_columns.f_showOnToolbar );
	col->add_attribute( rend->property_editable() , f_columns.f_showOnToolbar );
	//
	// Accel Key
	//
	col = get_column( col_num++ );
	col->set_sort_column( f_columns.f_accelKey );
	rend = dynamic_cast<Gtk::CellRendererText*>(*(col->get_cell_renderers().begin()));
	col->add_attribute( rend->property_sensitive(), f_columns.f_showOnToolbar );
	col->add_attribute( rend->property_editable() , f_columns.f_showOnToolbar );
	//
	// Show on Toolbar
	//
	col = get_column( col_num++ );
	col->set_sort_column( f_columns.f_showOnToolbar );
	Gtk::CellRendererToggle* trend = dynamic_cast<Gtk::CellRendererToggle*>(*(col->get_cell_renderers().begin()));
	trend->signal_toggled().connect( sigc::mem_fun( *this, &StatEditor::OnShowOnToolbarToggled ) );
	//
	// Show on HUD
	//
	col = get_column( col_num++ );
	col->set_sort_column( f_columns.f_showOnHUD );
	trend = dynamic_cast<Gtk::CellRendererToggle*>(*(col->get_cell_renderers().begin()));
	trend->signal_toggled().connect( sigc::mem_fun( *this, &StatEditor::OnShowOnHUDToggled ) );
	//
	// Show stat for monsters on HUD
	//
	col = get_column( col_num++ );
	col->set_sort_column( f_columns.f_showMonsterOnHUD );
	trend = dynamic_cast<Gtk::CellRendererToggle*>(*(col->get_cell_renderers().begin()));
	trend->signal_toggled().connect( sigc::mem_fun( *this, &StatEditor::OnShowMonsterOnHUDToggled ) );
	col->add_attribute( trend->property_sensitive()  , f_columns.f_showOnHUD );
	col->add_attribute( trend->property_activatable(), f_columns.f_showOnHUD );
		
	// Add all stats
	//
	CopyStats();
	AddStats();
}


StatEditor::~StatEditor()
{
}


bool StatEditor::on_key_press_event( GdkEventKey* event )
{
	return Gtk::TreeView::on_key_press_event(event);
}


void StatEditor::FixOrderings()
{
	// Fix orderings so they are contiguous
	//
	typedef std::map<int,Attribute::Stat::Pointer> StatMap;
	StatMap statMap;
	//
	for( auto stat : f_statsCopy )
	{
		assert(stat);
		statMap[stat->order()] = stat;
	}
	//
	f_statsCopy.clear();
	//
	int idx = 1;
	for( auto pair : statMap )
	{
		Attribute::Stat::Pointer	stat( pair.second );
		assert(stat);
		if( !stat->deleted() )
		{
			stat->order( idx++ );
		}
		//
		f_statsCopy.push_back( stat );
	}
}


void StatEditor::CopyStats()
{
	f_statsCopy.clear();
	//
	int idx = 1;
	for( auto pair : GetStatMgr().lock()->GetStats() )
	{
		Attribute::Stat::Pointer stat( pair.second );
		assert(stat);
		if( !stat->internal() && !stat->deleted() )
		{
			Attribute::Stat::Pointer new_stat( new Attribute::Stat );
			new_stat->Copy( stat );
			f_statsCopy.push_back( new_stat );
			//
			if( new_stat->order() == -1 )
			{
				new_stat->order( idx );
			}
		}
		idx++;
	}
}


void StatEditor::AddStats()
{
	// Make sure orderings are contiguous first
	//
	FixOrderings();

	// Clear out store first
	//
	Clear();
	for( auto stat : f_statsCopy )
	{
		assert(stat);
		if( !stat->deleted() )
		{
			AddStat( stat );
		}
	}
}


void StatEditor::AddStat( Attribute::Stat::Pointer stat )
{
	Gtk::TreeModel::Row row		= *(f_store->append());
	row[f_columns.f_stat]		= stat;

	PopulateRow( row );

	for( auto& row : f_accelKeyStore->children() )
	{
		Glib::ustring accel_key = row[f_accelKeyColumns.f_name];
		//
		if( molib::moWCString(accel_key.c_str()) != stat->accel() )
		{
			AddAccelKey( accel_key );
		}
	}
}


void StatEditor::AddDieFace( const Glib::ustring& value )
{
	Gtk::TreeModel::Row row = *(f_dieFacesStore->append());
	row[f_dieFacesColumns.f_name] = value;
}


void StatEditor::AddAccelKey( const Glib::ustring& value )
{
	Gtk::TreeModel::Row row = *(f_accelKeyStore->append());
	row[f_accelKeyColumns.f_name] = value;
}


void StatEditor::AddDieFacesColumn()
{
        Gtk::TreeView::Column* pColumn          = Gtk::manage( new Gtk::TreeView::Column("Die Faces") );
        Gtk::CellRendererCombo* pRenderer       = Gtk::manage( new Gtk::CellRendererCombo );
        pColumn->pack_start(*pRenderer);
        append_column(*pColumn);
		pColumn->add_attribute( pRenderer->property_text() , f_columns.f_dieFaces      );
        pRenderer->property_model()             = f_dieFacesStore;
        pRenderer->property_text_column()       = 0;
        pRenderer->property_editable()          = true;
		pRenderer->signal_edited().connect( sigc::mem_fun( *this, &StatEditor::OnDieFacesEdited ) );
		pColumn->set_sort_column( f_columns.f_dieFaces );

        // Standard die faces
        //
        AddDieFace( "1d2"   );
        AddDieFace( "1d4"   );
        AddDieFace( "1d6"   );
        AddDieFace( "2d6"   );
        AddDieFace( "1d8"   );
        AddDieFace( "1d10"  );
        AddDieFace( "1d12"  );
        AddDieFace( "1d20"  );
        AddDieFace( "1d30"  );
        AddDieFace( "1d100" );
 }


void StatEditor::AddAccelKeyColumn()
{
        Gtk::TreeView::Column* pColumn          = Gtk::manage( new Gtk::TreeView::Column("Accel key") );
        Gtk::CellRendererCombo* pRenderer       = Gtk::manage( new Gtk::CellRendererCombo );
        pColumn->pack_start(*pRenderer);
        append_column(*pColumn);
		pColumn->add_attribute( pRenderer->property_text() , f_columns.f_accelKey );
        pRenderer->property_model()             = f_accelKeyStore;
        pRenderer->property_text_column()       = 0;
		pRenderer->property_editable()          = true;
		pRenderer->signal_edited().connect( sigc::mem_fun( *this, &StatEditor::OnAccelKeyEdited ) );
		pColumn->set_sort_column( f_columns.f_accelKey );
}


#if 0
void StatEditor::AddAbilityColumn()
{
	Gtk::TreeView::Column* pColumn		= Gtk::manage( new Gtk::TreeView::Column("Ability") );
	Gtk::CellRendererCombo* pRenderer	= Gtk::manage( new Gtk::CellRendererCombo );
	pColumn->pack_start(*pRenderer);
	append_column(*pColumn);
	pColumn->add_attribute( pRenderer->property_model(), f_columns.f_abilityName );
	pRenderer->property_model()		= f_abilityStore;
	pRenderer->property_text_column()	= 0;
	pRenderer->property_editable()		= true;

	Gtk::TreeModel::Row row;
	row = *(f_abilityStore->append()); row[f_abilityColumns.f_name] = f_statMgr->Strength()    ->name().c_str();
	row = *(f_abilityStore->append()); row[f_abilityColumns.f_name] = f_statMgr->Dexterity()   ->name().c_str();
	row = *(f_abilityStore->append()); row[f_abilityColumns.f_name] = f_statMgr->Constitution()->name().c_str();
	row = *(f_abilityStore->append()); row[f_abilityColumns.f_name] = f_statMgr->Intelligence()->name().c_str();
	row = *(f_abilityStore->append()); row[f_abilityColumns.f_name] = f_statMgr->Wisdom()      ->name().c_str();
	row = *(f_abilityStore->append()); row[f_abilityColumns.f_name] = f_statMgr->Charisma()    ->name().c_str();
 
	show_all_children();
}
#endif


Attribute::Stat::Pointer StatEditor::GetOrCreateStat( const Gtk::TreeModel::Row& row )
{
	Attribute::Stat::Pointer stat = row[f_columns.f_stat];
	//
	if( !stat )
	{
		stat.reset( new Attribute::Stat );
		row[f_columns.f_stat] = stat;
		f_statsCopy.push_back( stat );
	}
	
	return stat;
}


void StatEditor::OnOrderEdited( const Glib::ustring& path, const Glib::ustring& new_text )
{
	assert(f_store);
	Gtk::TreeModel::iterator	iter	= f_store->get_iter( path );
	const Gtk::TreeModel::Row& 	row		= *(iter);
	//
	Attribute::Stat::Pointer stat = GetOrCreateStat( row );
	assert(stat);
	//
	stat->order( atoi( new_text.c_str() ) );
	//
	PopulateRow( iter );
}
																				
	
void StatEditor::OnNameEdited( const Glib::ustring& path, const Glib::ustring& new_text )
{
	assert(f_store);
	Gtk::TreeModel::iterator	iter	= f_store->get_iter( path );
	const Gtk::TreeModel::Row& 	row		= *(iter);
	//
	Attribute::Stat::Pointer stat = GetOrCreateStat( row );
	assert(stat);
	//
	bool dup = false;
	for( auto copy_stat : f_statsCopy )
	{
		assert(copy_stat);
		if(	copy_stat != stat 
				  && !copy_stat->deleted()
				  && copy_stat->name() == molib::moWCString(new_text.c_str()) )
		{
			dup = true;
			break;		
		}
	}
	//
	if( dup )
	{
		Gtk::MessageDialog dialog( 
			"Duplicate name. Please enter a unique value.",
			false, Gtk::MESSAGE_ERROR );
		dialog.run();
		set_cursor( Gtk::TreePath(path), *f_nameColumn, true );
	}
	else
	{	
		stat->name( new_text.c_str() );
	}
	//
	PopulateRow( iter );
}
	

void StatEditor::OnDieFacesEdited( const Glib::ustring& path, const Glib::ustring& new_text )
{
	assert(f_store);
	Gtk::TreeModel::iterator	iter	= f_store->get_iter( path );
	const Gtk::TreeModel::Row& 	row		= *(iter);
	//
	Attribute::Stat::Pointer stat = GetOrCreateStat( row );
	assert(stat);
	//
	int dice, faces, modifier;
	Common::StrToDiceFacesModifier( new_text.c_str(), dice, faces, modifier );
	stat->dice( dice );
	stat->faces( faces );
	stat->modifier( modifier );
	//
	PopulateRow( iter );
}


void StatEditor::OnAccelKeyEdited( const Glib::ustring& path, const Glib::ustring& new_text )
{
	assert(f_store);
	Gtk::TreeModel::iterator	iter	= f_store->get_iter( path );
	const Gtk::TreeModel::Row& 	row		= *(iter);
	//
	Attribute::Stat::Pointer stat = GetOrCreateStat( row );
	assert(stat);
	//
	stat->accel( new_text.c_str() );
	//
	PopulateRow( iter );
}


void StatEditor::OnShowMonsterOnHUDToggled( const Glib::ustring& path )
{
	assert(f_store);
	Gtk::TreeModel::iterator	iter		= f_store->get_iter( path );
	const Gtk::TreeModel::Row& 	row			= *(iter);
	const bool					selection	= row[f_columns.f_showMonsterOnHUD];
	//
	Attribute::Stat::Pointer stat = GetOrCreateStat( row );
	assert(stat);
	stat->showMonsterOnHUD( !selection );
	//
	PopulateRow( iter );
}


void StatEditor::OnShowOnToolbarToggled( const Glib::ustring& path )
{
	assert(f_store);
	Gtk::TreeModel::iterator	iter		= f_store->get_iter( path );
	const Gtk::TreeModel::Row& 	row			= *(iter);
	const bool					selection	= row[f_columns.f_showOnToolbar];
	//
	Attribute::Stat::Pointer stat = GetOrCreateStat( row );
	assert(stat);
	stat->showOnToolbar( !selection );
	//
	PopulateRow( iter );
}


void StatEditor::OnShowOnHUDToggled( const Glib::ustring& path )
{
	assert(f_store);
	Gtk::TreeModel::iterator	iter		= f_store->get_iter( path );
	const Gtk::TreeModel::Row& 	row			= *(iter);
	const bool					selection	= row[f_columns.f_showOnHUD];
	//
	Attribute::Stat::Pointer stat = GetOrCreateStat( row );
	assert(stat);
	stat->showOnHUD( !selection );
	//
	PopulateRow( iter );
}


bool StatEditor::PopulateRow( const Gtk::TreeModel::iterator& iter )
{
	const Gtk::TreeModel::Row& row = *iter;
	Attribute::Stat::Pointer stat = row[f_columns.f_stat];
	assert(stat);

	row[f_columns.f_order]				= stat->order();
	row[f_columns.f_name]				= stat->name().c_str();
	row[f_columns.f_showOnToolbar]		= stat->showOnToolbar();
	row[f_columns.f_showOnHUD]			= stat->showOnHUD();
	row[f_columns.f_showMonsterOnHUD]	= stat->showMonsterOnHUD();
	
	if( stat->showOnToolbar() )
	{
		row[f_columns.f_dieFaces]		= Common::DiceFacesModifierToStr( stat->dice(), stat->faces(), stat->modifier() ).c_str();
		row[f_columns.f_accelKey]		= stat->accel().c_str();
	}
	else
	{
		const char* NA					= gettext("N/A");
		row[f_columns.f_dieFaces]		= NA;
		row[f_columns.f_accelKey]		= NA;
	}

	return false;
}


void StatEditor::Clear()
{
	f_store = Gtk::ListStore::create( f_columns );
	assert(f_store);
	//
	set_model( f_store );
	//
	f_store->set_sort_column( f_columns.f_order, Gtk::SORT_ASCENDING );
}


void StatEditor::Apply()
{
    const auto statMgr(GetStatMgr().lock());
    assert(statMgr);
    const auto& map = statMgr->GetStats();
	//
	for( auto copy_stat : f_statsCopy )
	{
		assert(copy_stat);
		//
		bool found = false;
		for( auto pair : map )
		{
			Attribute::Stat::Pointer stat( pair.second );
			assert(stat);
			//
			if( stat->id() == copy_stat->id() )
			{
				if( copy_stat->deleted() )
				{
					stat->deleted(true);
				}
				else
				{
					stat->Copy( copy_stat );
				}
				//
				found = true;
				break;
			}
		}
		//
		if( !found )
		{
			// Newly added, so append
			//
			copy_stat->id( molib::moName( copy_stat->name() ) );
            statMgr->AddStat( copy_stat );
		}
	}
}


void StatEditor::AddNewStat()
{
	Gtk::TreeModel::iterator	new_iter = f_store->append();
	Gtk::TreeModel::Row			row = *new_iter;
	//
	FixOrderings();
	//
	Attribute::Stat::Pointer	new_stat = GetOrCreateStat( row );
	//
	new_stat->order( f_statsCopy.size() );
	new_stat->name( "New Entry" );
	new_stat->dice( 1 );
	new_stat->faces( 20 );
	new_stat->showOnToolbar( true );
	new_stat->showOnHUD( false );
	new_stat->showMonsterOnHUD( false );
	//
	row[f_columns.f_stat] = new_stat;
	PopulateRow( new_iter );
	//
	Gtk::TreeModel::Path		path = f_store->get_path( new_iter );
	set_cursor( path, *f_nameColumn, true );
}


void StatEditor::MoveSelected( const bool up )
{
	typedef std::map<int,Attribute::Stat::Pointer> OrderStatMap;
	OrderStatMap orderMap;
	
	// First load each stat into the map, sorted by order
	//
	for( auto copy_stat : f_statsCopy )
	{
		assert(copy_stat);
		orderMap[copy_stat->order()] = copy_stat;
	}
	
	// Second, iterate through the selected members and reorder into the map
	//
	for( auto path : get_selection()->get_selected_rows() )
	{
		Gtk::TreeModel::iterator	list_model_iter	( f_store->get_iter( path ) );
		Gtk::TreeRow				row				( *(list_model_iter)        );
		Attribute::Stat::Pointer 	stat 			( row[f_columns.f_stat]     );
		assert(stat);
		//
		const int cur_order = stat->order();
		//
		if( (up && cur_order == 1) || (!up && cur_order == orderMap.size()) )
		{
			continue;
		}
		//
		const int new_order = up ? cur_order - 1: cur_order + 1;
		//
		Attribute::Stat::Pointer swap_stat = orderMap[new_order];
		orderMap[new_order] = orderMap[cur_order];
		orderMap[cur_order] = swap_stat;
	}
	
	// Third, change the stat order based on the new order in the map
	//
	for( auto pair : orderMap )
	{
		Attribute::Stat::Pointer stat( pair.second );
		assert(stat);
        stat->order( pair.first );
	}

	FixOrderings();

	// Now update the list
	//
	assert(f_store);
	auto		model_iter ( f_store->children().begin() );
	const auto	model_end  ( f_store->children().end()   );
	//
	for( ; model_iter != model_end; ++model_iter )
	{
		PopulateRow( model_iter );
	}
}


void StatEditor::DeleteSelected()
{
	assert(f_store);
	for( auto path : get_selection()->get_selected_rows() )
	{
		Gtk::TreeModel::iterator list_model_iter ( f_store->get_iter(path) );
		Gtk::TreeRow             row             ( *(list_model_iter)      );
		Attribute::Stat::Pointer stat            ( row[f_columns.f_stat]   );
		assert(stat);
		stat->deleted( true );
	}
	
	AddStats();
}


}
// namespace UI

// vim: ts=4 sw=4 syntax=cpp.doxygen

