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
//
#include "base/SoftColumns.h"

// molib
//
#include "mo/mo_name.h"

#include <memory>
#include <map>

namespace Combatant
{

class CharacterColumns : public SoftColumns
{
public:
	typedef std::shared_ptr<CharacterColumns>       pointer_t;

    CharacterColumns();
	~CharacterColumns();

	// I'm a bit nervous about this ugly static cast. For some reason,
	// C++ won't allow me to dynamically cast from the base class.
	//
	template<class T>
        T& GetField( const molib::mo_name_t& id )
		{
			return *(std::static_pointer_cast<T>(GetColumn(id)).get());
		}

	//===============================================================================
	// Row column lookups
	//
	col_ustring_t     GetName()            ;
	col_ustring_t     GetPublicName()      ;
	col_ustring_t     GetHpDisplay()       ;
	col_ustring_t     GetStatus()          ;
	col_pixbuf_t      GetArrowIcon()       ;
	col_pixbuf_t      GetCharacterIcon()   ;
	col_int_t         GetInit()            ;
	col_ustring_t     GetEffects()         ;
	col_int_t         GetHitPoints()       ;
	col_int_t         GetHpPercent()       ;
	col_bool_t        GetMonster()         ;
	col_int_t         GetPosition()        ;
	col_int_t         GetOrder()           ;
	col_character_t   GetCharacter()       ;
	col_connection_t  GetConnection()      ;
	col_bool_t        GetEditable()        ;
	col_bool_t        GetSensitive()       ;
	col_ustring_t     GetBackgroundColor() ;
	col_ustring_t     GetForegroundColor() ;
	col_weight_t      GetWeight()          ;
	col_style_t       GetStyle()           ;
	//
	col_int_t&    	  GetSoftCol             ( const molib::mo_name_t& id );
	col_ustring_t& 	  GetSoftColString       ( const molib::mo_name_t& id );
	col_weight_t&  	  GetSoftBold            ( const molib::mo_name_t& id );
	col_ustring_t& 	  GetSoftBackgroundColor ( const molib::mo_name_t& id );
	col_ustring_t& 	  GetSoftForegroundColor ( const molib::mo_name_t& id );

	//===============================================================================
	// View column lookups
	//
	column_t& GetNameCol()            ;
	column_t& GetPublicNameCol()      ;
	column_t& GetHpDisplayCol()       ;
	column_t& GetStatusCol()          ;
	column_t& GetArrowIconCol()       ;
	column_t& GetCharacterIconCol()   ;
	column_t& GetInitCol()            ;
	column_t& GetEffectsCol()         ;
	column_t& GetHitPointsCol()       ;
	column_t& GetHpPercentCol()       ;
	column_t& GetMonsterCol()         ;
	column_t& GetPositionCol()        ;
	column_t& GetOrderCol()           ;
	column_t& GetCharacterCol()       ;
	column_t& GetConnectionCol()      ;
	column_t& GetEditableCol()        ;
	column_t& GetSensitiveCol()       ;
	column_t& GetBackgroundColorCol() ;
	column_t& GetForegroundColorCol() ;
	column_t& GetWeightCol()          ;
	column_t& GetStyleCol()           ;
	//
	column_t& GetSoftColCol             ( const molib::mo_name_t& id ) ;
	column_t& GetSoftColStringCol       ( const molib::mo_name_t& id ) ;
	column_t& GetSoftBoldCol            ( const molib::mo_name_t& id ) ;
	column_t& GetSoftBackgroundColorCol ( const molib::mo_name_t& id ) ;
	column_t& GetSoftForegroundColorCol ( const molib::mo_name_t& id ) ;

private:
	struct ColEffects;
	typedef std::map<molib::mo_name_t,ColEffects>	effects_map_t;

	molib::mo_name_t f_hp_display          ;
	molib::mo_name_t f_hp_percent          ;
	molib::mo_name_t f_name                ;
	molib::mo_name_t f_publicName          ;
	molib::mo_name_t f_status              ;
	molib::mo_name_t f_arrow_icon          ;
	molib::mo_name_t f_character_icon      ;
	molib::mo_name_t f_init                ;
	effects_map_t    f_softCols            ;
	molib::mo_name_t f_effects             ;
	molib::mo_name_t f_hit_points          ;
	molib::mo_name_t f_monster             ;
	molib::mo_name_t f_position            ;
	molib::mo_name_t f_order               ; // Display version of position (but opposite)
	molib::mo_name_t f_character           ;
	molib::mo_name_t f_char_connection     ;
	molib::mo_name_t f_editable            ;
	molib::mo_name_t f_sensitive           ;
	molib::mo_name_t f_background_color    ;
	molib::mo_name_t f_foreground_color    ;
	molib::mo_name_t f_weight              ;
	molib::mo_name_t f_style               ;

	void InitSoftColumns();
};

}
// namespace Combatant

// vim: ts=4 sw=4 syntax=cpp.doxygen

