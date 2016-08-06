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


}
// namespace UI


// vim: ts=4 sw=4

