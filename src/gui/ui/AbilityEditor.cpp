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
#include "AbilityEditor.h"

using namespace Attribute;

namespace UI
{

AbilityEditor::AbilityEditor()
{
	assert(GetStatMgr().lock());
	//
	f_store = Gtk::ListStore::create( f_columns );
	assert(f_store);
	set_model( f_store );

	//f_dieFacesStore = Gtk::ListStore::create( f_dieFacesColumns );
	//assert(f_dieFacesStore);

	append_column( "Name"        ,  f_columns.f_name        );
	append_column( "Display Name",  f_columns.f_displayName );

	InitDialog();
	//UpdateDialog();
}


AbilityEditor::~AbilityEditor()
{
}


#if 0
void AbilityEditor::AddDieFace( const Glib::ustring& value )
{
	Gtk::TreeModel::Row row = *(f_dieFacesStore->append());
	row[f_dieFacesColumns.f_dieFace] = value;
}


void AbilityEditor::AddDieFaceColumn()
{
	Gtk::TreeView::Column* pColumn          = Gtk::manage( new Gtk::TreeView::Column("Die Faces") );
	Gtk::CellRendererCombo* pRenderer       = Gtk::manage( new Gtk::CellRendererCombo );
	pColumn->pack_start(*pRenderer);
	append_column(*pColumn);
	pColumn->add_attribute( pRenderer->property_model(), f_columns.f_dieFaces );
	pRenderer->property_model()             = f_dieFacesStore;
	pRenderer->property_text_column()       = 0;
	pRenderer->property_editable()          = true;

	// Standard die faces
	//
	AddDieFace( "2"   );
	AddDieFace( "4"   );
	AddDieFace( "6"   );
	AddDieFace( "8"   );
	AddDieFace( "10"  );
	AddDieFace( "12"  );
	AddDieFace( "20"  );
	AddDieFace( "30"  );
	AddDieFace( "100" );
}
#endif


void AbilityEditor::AddAbility( const Glib::ustring& name, Stat::pointer_t stat )
{
	Gtk::TreeModel::Row row = *(f_store->append());
	row[f_columns.f_name       ] = name;
	row[f_columns.f_displayName] = stat->name().c_str();
	row[f_columns.f_ability    ] = stat;
}


void AbilityEditor::InitDialog()
{
	ClearDialog();

	auto statMgr = GetStatMgr().lock();
	AddAbility( gettext("Strength")     , statMgr->Strength()     );
	AddAbility( gettext("Dexterity")    , statMgr->Dexterity()    );
	AddAbility( gettext("Constitution") , statMgr->Constitution() );
	AddAbility( gettext("Intelligence") , statMgr->Intelligence() );
	AddAbility( gettext("Wisdom")       , statMgr->Wisdom()       );
	AddAbility( gettext("Charisma")     , statMgr->Charisma()     );
}


void AbilityEditor::ClearDialog()
{
	// TODO
}


void AbilityEditor::Apply()
{
	for( auto row : f_store->children() )
	{
		Stat::pointer_t	ability	     = row[f_columns.f_ability];
		Glib::ustring	display_name = row[f_columns.f_displayName];
		ability->name( display_name.c_str() );
	}
}


void AbilityEditor::Revert()
{
	for( auto row : f_store->children() )
	{
		Stat::pointer_t ability = row[f_columns.f_ability];
		row[f_columns.f_displayName] = ability->name().c_str();
	}
}

#if 0
void AbilityEditor::on_cursor_changed()
{
	std::cout << "on_cursor_changed()" << std::endl;
	if( !f_editingStarted )
	{
		f_TreeView.get_cursor( f_currentPath, f_currentCol );
	}
}


void AbilityEditor::on_editing_started( Gtk::CellEditable*, const Glib::ustring& )
{
	std::cout << "on_editing_started()" << std::endl;
	f_editingStarted = true;
}


void AbilityEditor::on_editing_canceled()
{
	std::cout << "on_editing_canceled()" << std::endl;
	f_editingStarted = false;
}


void AbilityEditor::NextTabRow( const bool forward )
{
	Gtk::TreeNodeChildren    children = f_store->children();
	Gtk::TreeModel::iterator iter     = f_store->get_iter( f_currentPath );
	Gtk::TreeModel::iterator begin    = children.begin();
	Gtk::TreeModel::iterator end      = children.end();
	assert(iter);

	if( forward )
	{
		++iter;
		if( iter == end )
		{
			iter = children.begin();
		}
	}
	else
	{
		if( iter == begin )
		{
			iter = children.end();
		}
		--iter;
	}

	f_currentPath = f_refTreeModel->get_path( iter );
}


void AbilityEditor::NextTabColumn( const bool forward )
{
	TreeViewColumns           columns = f_TreeView.get_columns();
	TreeViewColumns::iterator begin   = columns.begin();
	TreeViewColumns::iterator end     = columns.end();
	TreeViewColumns::iterator iter    = begin;
	assert(iter);

	for( ; iter != end; ++iter )
	{
		if( *iter == f_currentCol )
		{
			break;
		}
	}

	assert( iter );

	if( forward )
	{
		++iter;
		if( iter == end )
		{
			iter = children.begin();
		}
	}
	else
	{
		if( iter == begin )
		{
			iter = children.end();
		}
		--iter;
	}

	f_currentPath = f_refTreeModel->get_path( iter );
}

void AbilityEditor::DoTab( const bool forward );
{
	NextTabRow( forward );
	set_cursor( f_currentPath, *f_currentCol, true /* start_editing */ );
}


bool AbilityEditor::on_key_press_event( GdkEventKey* event )
{
	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	bool return_value = Gtk::Window::on_key_press_event(event);

	// Emit the OK response on ENTER
	//
	if( f_editingStarted )
	{
		switch( event->keyval )
		{
			case GDK_Tab:
			case GDK_Return:
				DoTab( true );
				break;

			case GDK_ISO_Left_Tab:
				DoTab( false );
				break;
		}
	}

	return return_value;
}
#endif

}
// namespace UI


// vim: ts=4 sw=4

