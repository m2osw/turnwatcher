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

// Local includes
#include "SoftColumns.h"

#include <memory>
#include <map>

namespace Combatant
{

class CharacterColumns : public SoftColumns
{
public:
	typedef std::shared_ptr<CharacterColumns> Pointer;

	CharacterColumns();
	~CharacterColumns();

	Gtk::TreeModelColumn<Glib::ustring>* GetName()          const;
	Gtk::TreeModelColumn<Glib::ustring>* GetHpDisplay()     const;
	Gtk::TreeModelColumn<Glib::ustring>* GetStatus()        const;
	Gtk::TreeModelColumn<PixbufPtr>*     GetArrowIcon()     const;
	Gtk::TreeModelColumn<PixbufPtr>*     GetCharacterIcon() const;
	Gtk::TreeModelColumn<int>*           GetInit()          const;
	//
	Gtk::TreeModelColumn<int>*           GetSoftCol             ( const molib::mo_name_t id );
	Gtk::TreeModelColumn<Glib::ustring>* GetSoftColString       ( const molib::mo_name_t id );
	Gtk::TreeModelColumn<Pango::Weight>* GetSoftBold            ( const molib::mo_name_t id );
	Gtk::TreeModelColumn<Glib::ustring>* GetSoftBackgroundColor ( const molib::mo_name_t id );
	Gtk::TreeModelColumn<Glib::ustring>* GetSoftForegroundColor ( const molib::mo_name_t id );
	//
#if defined(WANT_EFFECTS)
	Gtk::TreeModelColumn<Glib::ustring>*                 GetEffects()         const;
#endif
	Gtk::TreeModelColumn<int>*                           GetHitPoints()       const;
	Gtk::TreeModelColumn<int>*                           GetHpPercent()       const;
	Gtk::TreeModelColumn<bool>*                          GetMonster()         const;
	//
	Gtk::TreeModelColumn<int>*                           GetPosition()        const;
	Gtk::TreeModelColumn<int>*                           GetOrder()           const;
	Gtk::TreeModelColumn<Combatant::Character::Pointer>* GetCharacter()       const;
	Gtk::TreeModelColumn<sigc::connection>*              GetConnection()      const;
	//
	Gtk::TreeModelColumn<bool>*                          GetEditable()        const;
	Gtk::TreeModelColumn<bool>*                          GetSensitive()       const;
	Gtk::TreeModelColumn<Glib::ustring>*                 GetBackgroundColor() const;
	Gtk::TreeModelColumn<Glib::ustring>*                 GetForegroundColor() const;
	Gtk::TreeModelColumn<Pango::Weight>*                 GetWeight()          const;
	Gtk::TreeModelColumn<Pango::Style>*                  GetStyle()           const;
	
	//int GetSoftColId( const molib::mo_name_t id );

private:
	int			f_hp_display    ;
	int			f_hp_percent	;
	int			f_name	      	;
	int			f_status	  	;
	int			f_arrow_icon    ;
	int			f_character_icon;
	int			f_init	      	;

	struct ColEffects
	{
		int		f_column;
		int		f_string;
		int		f_bold;
		int		f_foreground_color;
		int		f_background_color;
		//
		ColEffects() : f_column(0), f_string(0), f_bold(0), f_foreground_color(0), f_background_color(0)
		{
		}
	};
	typedef std::map<molib::mo_name_t,ColEffects>	ColEffectsMap;
	ColEffectsMap									f_softCols;

#if defined(WANT_EFFECTS)
	int			f_effects	  		;
#endif
	int			f_hit_points  		;
	int			f_monster	  		;
	//
	int			f_position	  		;
	int			f_order				;	// Display version of position (but opposite)
	int			f_character	  		;
	int			f_char_connection	;
	//
	int			f_editable	  		;
	int			f_sensitive       	;
	int			f_background_color	;
	int			f_foreground_color	;
	int			f_weight	  		;
	int			f_style	      	  	;
};

}
// namespace Combatant

// vim: ts=4 sw=4 syntax=cpp.doxygen

