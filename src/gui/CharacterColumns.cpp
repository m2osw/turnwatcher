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




#include "CharacterColumns.h"
#include "SingletonManager.h"

#include <vector>

namespace Combatant
{

CharacterColumns::CharacterColumns() :
	f_hp_display    	(-1),
	f_hp_percent		(-1),
	f_name	      		(-1),
	f_status	  		(-1),
	f_arrow_icon    	(-1),
	f_character_icon	(-1),
	f_init	      		(-1),
#if defined(WANT_EFFECTS)
	f_effects	  		(-1),
#endif
	f_hit_points  		(-1),
	f_monster	  		(-1),
	f_position	  		(-1),
	f_order				(-1),
	f_character	  		(-1),
	f_char_connection	(-1),
	f_editable	  		(-1),
	f_sensitive       	(-1),
	f_background_color	(-1),
	f_foreground_color	(-1),
	f_weight	  		(-1),
	f_style	      	  	(-1)
{
	f_hp_display      	= AddColumn( CharacterColumns::StringColumn	() );
	f_hp_percent		= AddColumn( CharacterColumns::IntColumn	() );
	f_name	      		= AddColumn( CharacterColumns::StringColumn	() ); 
	f_status			= AddColumn( CharacterColumns::StringColumn	() ); 
	f_arrow_icon      	= AddColumn( CharacterColumns::PixbufColumn	() ); 
	f_character_icon  	= AddColumn( CharacterColumns::PixbufColumn	() ); 
	f_init	      		= AddColumn( CharacterColumns::IntColumn	() ); 

	auto map = Application::Manager::Instance().lock()->GetStatMgr().lock()->GetStats();
	//
	for( auto statPair : map )
	{
		auto stat( statPair.second );
		if( !stat->internal() && !stat->deleted() )
		{
			const molib::mo_name_t id = stat->id();
			//
			ColEffects& ce			= f_softCols[id];
			ce.f_column				= AddColumn( SoftColumns::IntColumn   () ); 
			ce.f_string				= AddColumn( SoftColumns::StringColumn() );
			ce.f_bold				= AddColumn( SoftColumns::WeightColumn() ); 
			ce.f_foreground_color	= AddColumn( SoftColumns::StringColumn() ); 
			ce.f_background_color	= AddColumn( SoftColumns::StringColumn() ); 
		}
	}

#if defined(WANT_EFFECTS)
	f_effects			= AddColumn( SoftColumns::StringColumn		() ); 
#endif
	f_hit_points      	= AddColumn( SoftColumns::IntColumn			() ); 
	f_monster	      	= AddColumn( SoftColumns::BoolColumn		() ); 
	//
	f_position	      	= AddColumn( SoftColumns::IntColumn			() );
	f_order				= AddColumn( SoftColumns::IntColumn			() );
	f_character	      	= AddColumn( SoftColumns::CharacterColumn	() ); 
	f_char_connection	= AddColumn( SoftColumns::ConnectionColumn	() ); 
	//
	f_editable			= AddColumn( SoftColumns::BoolColumn		() );
	f_sensitive       	= AddColumn( SoftColumns::BoolColumn		() ); 
	f_background_color	= AddColumn( SoftColumns::StringColumn		() ); 
	f_foreground_color	= AddColumn( SoftColumns::StringColumn		() ); 
	f_weight			= AddColumn( SoftColumns::WeightColumn		() ); 
	f_style	      		= AddColumn( SoftColumns::StyleColumn		() ); 
}


CharacterColumns::~CharacterColumns()
{
	// Empty
}


#if 0
int CharacterColumns::GetSoftColId( const molib::mo_name_t id )
{
	return f_softCols[id].f_column;
}
#endif
	

Gtk::TreeModelColumn<Glib::ustring>*	CharacterColumns::GetName()	 const
{
	return GetStringColumn(GetColumn( f_name ));
}


Gtk::TreeModelColumn<Glib::ustring>*	CharacterColumns::GetHpDisplay() const
{
	return GetStringColumn   (GetColumn(f_hp_display	));
}


Gtk::TreeModelColumn<Glib::ustring>*	CharacterColumns::GetStatus() const
{
	return GetStringColumn   (GetColumn(f_status    	));
}


Gtk::TreeModelColumn<CharacterColumns::PixbufPtr>* CharacterColumns::GetArrowIcon() const
{
	return GetPixbufColumn   (GetColumn(f_arrow_icon	));
}


Gtk::TreeModelColumn<CharacterColumns::PixbufPtr>* CharacterColumns::GetCharacterIcon()	const
{
	return GetPixbufColumn   (GetColumn(f_character_icon));
}


Gtk::TreeModelColumn<int>* 	CharacterColumns::GetInit() const
{
	return GetIntColumn      (GetColumn(f_init			));
}


Gtk::TreeModelColumn<int>* CharacterColumns::GetSoftCol( const molib::mo_name_t id )	
{
	return GetIntColumn( GetColumn( f_softCols[id].f_column ) );
}


Gtk::TreeModelColumn<Glib::ustring>* CharacterColumns::GetSoftColString( const molib::mo_name_t id )	
{
	return GetStringColumn( GetColumn( f_softCols[id].f_string ) );
}


Gtk::TreeModelColumn<Pango::Weight>* CharacterColumns::GetSoftBold( const molib::mo_name_t id )
{
	return GetWeightColumn( GetColumn( f_softCols[id].f_bold ) );
}


Gtk::TreeModelColumn<Glib::ustring>* CharacterColumns::GetSoftBackgroundColor( const molib::mo_name_t id )
{
	return GetStringColumn( GetColumn( f_softCols[id].f_background_color ) );
}


Gtk::TreeModelColumn<Glib::ustring>* CharacterColumns::GetSoftForegroundColor( const molib::mo_name_t id )
{
	return GetStringColumn( GetColumn( f_softCols[id].f_foreground_color ) );
}


#if defined(WANT_EFFECTS)
Gtk::TreeModelColumn<Glib::ustring>*	CharacterColumns::GetEffects() const
{
	return GetStringColumn   (GetColumn(f_effects   	));
}
#endif


Gtk::TreeModelColumn<int>* 	CharacterColumns::GetHitPoints() const
{
	return GetIntColumn      (GetColumn(f_hit_points	));
}


Gtk::TreeModelColumn<int>* 	CharacterColumns::GetHpPercent() const
{
	return GetIntColumn      (GetColumn(f_hp_percent	));
}


Gtk::TreeModelColumn<bool>* CharacterColumns::GetMonster() const
{
	return GetBoolColumn     (GetColumn(f_monster		));
}


Gtk::TreeModelColumn<int>* 	CharacterColumns::GetPosition() const
{
	return GetIntColumn      (GetColumn(f_position		));
}


Gtk::TreeModelColumn<int>* 	CharacterColumns::GetOrder() const
{
	return GetIntColumn      (GetColumn(f_order));
}


Gtk::TreeModelColumn<Combatant::Character::Pointer>* CharacterColumns::GetCharacter() const
{
	return GetCharacterColumn(GetColumn(f_character));
}


Gtk::TreeModelColumn<sigc::connection>*	CharacterColumns::GetConnection() const
{
	return GetConnectionColumn(GetColumn(f_char_connection	));
}


Gtk::TreeModelColumn<bool>*	CharacterColumns::GetEditable() const
{
	return GetBoolColumn     (GetColumn(f_editable		));
}


Gtk::TreeModelColumn<bool>*	CharacterColumns::GetSensitive() const
{
	return GetBoolColumn     (GetColumn(f_sensitive		));
}


Gtk::TreeModelColumn<Glib::ustring>*	CharacterColumns::GetBackgroundColor()	const
{
	return GetStringColumn   (GetColumn(f_background_color	));
}


Gtk::TreeModelColumn<Glib::ustring>*	CharacterColumns::GetForegroundColor()	const
{
	return GetStringColumn   (GetColumn(f_foreground_color	));
}


Gtk::TreeModelColumn<Pango::Weight>*	CharacterColumns::GetWeight()	 const
{
	return GetWeightColumn   (GetColumn(f_weight		));
}


Gtk::TreeModelColumn<Pango::Style>* CharacterColumns::GetStyle()	 const
{
	return GetStyleColumn    (GetColumn(f_style		));
}


}
// namespace Combatant

// vim: ts=4 sw=4 syntax=cpp.doxygen

