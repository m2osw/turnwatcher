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

#include "base/CharacterColumns.h"
#include "base/StatManager.h"

#include <vector>

using namespace molib;
using namespace Attribute;

namespace Combatant
{

class CharacterColumns::ColEffects
{
public:
    ColEffects() {}
    ColEffects( const mo_name_t& stat_id )
		: f_column           ( moName( static_cast<moWCString>(moName(stat_id)) + "_EFFECTS_COLUMN"     ) )
		, f_string           ( moName( static_cast<moWCString>(moName(stat_id)) + "_EFFECTS_STRING"     ) )
		, f_bold             ( moName( static_cast<moWCString>(moName(stat_id)) + "_EFFECTS_BOLD"       ) )
		, f_foreground_color ( moName( static_cast<moWCString>(moName(stat_id)) + "_EFFECTS_FOREGROUND" ) )
		, f_background_color ( moName( static_cast<moWCString>(moName(stat_id)) + "_EFFECTS_BACKGROUND" ) )
	{}

    const mo_name_t& GetColumn          () const { return f_column;           }
    const mo_name_t& GetString          () const { return f_string;           }
    const mo_name_t& GetBold            () const { return f_bold;             }
    const mo_name_t& GetForegroundColor () const { return f_foreground_color; }
    const mo_name_t& GetBackgroundColor () const { return f_background_color; }

private:
    mo_name_t		f_column;
    mo_name_t		f_string;
    mo_name_t		f_bold;
    mo_name_t		f_foreground_color;
    mo_name_t		f_background_color;
};


CharacterColumns::CharacterColumns()
	: f_hp_display          (moName("COL_HP_DISPLAY"))
	, f_hp_percent          (moName("COL_HP_PERCENT"))
	, f_name                (moName("COL_NAME"))
	, f_publicName          (moName("COL_PUBLIC_NAME"))
	, f_status              (moName("COL_STATUS"))
	, f_arrow_icon          (moName("COL_ARROW_ICON"))
	, f_character_icon      (moName("COL_CHAR_ICON"))
	, f_init                (moName("COL_INIT"))
	, f_effects             (moName("COL_EFFECTS"))
	, f_hit_points          (moName("COL_HIT_POINTS"))
	, f_monster             (moName("COL_MONSTER"))
	, f_position            (moName("COL_POSITION"))
	, f_order               (moName("COL_ORDER"))
	, f_character           (moName("COL_CHAR"))
	, f_char_connection     (moName("COL_CHARCONN"))
	, f_editable            (moName("COL_EDITABLE"))
	, f_sensitive           (moName("COL_SENSITIVE"))
	, f_background_color    (moName("COL_BACKGROUND"))
	, f_foreground_color    (moName("COL_FOREGROUNC"))
	, f_weight              (moName("COL_WEIGHT"))
	, f_style               (moName("COL_STYLE"))
{
	// Visible columns to the user
	//
	AddColumn( f_order          , column_ptr_t(new col_int_t     ()) );
	AddColumn( f_arrow_icon     , column_ptr_t(new col_pixbuf_t  ()) );
	AddColumn( f_name           , column_ptr_t(new col_ustring_t ()) );
	AddColumn( f_publicName     , column_ptr_t(new col_ustring_t ()) );
	AddColumn( f_character_icon , column_ptr_t(new col_pixbuf_t  ()) );
	AddColumn( f_monster        , column_ptr_t(new col_bool_t    ()) );
	AddColumn( f_init           , column_ptr_t(new col_int_t     ()) );
	AddColumn( f_hp_display     , column_ptr_t(new col_ustring_t ()) );
	AddColumn( f_hp_percent     , column_ptr_t(new col_int_t     ()) );
	AddColumn( f_status         , column_ptr_t(new col_ustring_t ()) );
	AddColumn( f_effects        , column_ptr_t(new col_ustring_t ()) );
	//
    // Hidden columns which carry information
    //
    AddColumn( f_hit_points       , column_ptr_t(new col_int_t        ()) );
    AddColumn( f_position         , column_ptr_t(new col_int_t        ()) );
    AddColumn( f_character        , column_ptr_t(new col_character_t  ()) );
    AddColumn( f_char_connection  , column_ptr_t(new col_connection_t ()) );
    AddColumn( f_editable         , column_ptr_t(new col_bool_t       ()) );
    AddColumn( f_sensitive        , column_ptr_t(new col_bool_t       ()) );
    AddColumn( f_background_color , column_ptr_t(new col_ustring_t    ()) );
    AddColumn( f_foreground_color , column_ptr_t(new col_ustring_t    ()) );
    AddColumn( f_weight           , column_ptr_t(new col_weight_t     ()) );
    AddColumn( f_style            , column_ptr_t(new col_style_t      ()) );
	//
	// Custom columns that the user may add/delete/modify
	//
	InitSoftColumns();
}


CharacterColumns::~CharacterColumns()
{
	// Empty
}


void CharacterColumns::InitSoftColumns()
{
	f_softCols.clear();

	auto map( StatManager::Instance().lock()->GetStats() );
	//
	for( auto statPair : map )
	{
		auto stat( statPair.second );
		if( !stat->internal() && !stat->deleted() )
		{
            ColEffects ce( stat->id() );
            //
            AddColumn( ce.GetColumn          (), column_ptr_t(new col_int_t   ()) );
            AddColumn( ce.GetString          (), column_ptr_t(new col_ustring_t()) );
            AddColumn( ce.GetBold            (), column_ptr_t(new col_weight_t()) );
            AddColumn( ce.GetForegroundColor (), column_ptr_t(new col_ustring_t()) );
            AddColumn( ce.GetBackgroundColor (), column_ptr_t(new col_ustring_t()) );
            //
            f_softCols[stat->id()] = ce;
        }
	}
}


//===============================================================================
// Model column lookups
//
// Fixed columns
//
CharacterColumns::col_ustring_t    CharacterColumns::GetName()            { return GetField<col_ustring_t    >(f_name             ); }
CharacterColumns::col_ustring_t    CharacterColumns::GetPublicName()      { return GetField<col_ustring_t    >(f_publicName       ); }
CharacterColumns::col_ustring_t    CharacterColumns::GetHpDisplay()       { return GetField<col_ustring_t    >(f_hp_display       ); }
CharacterColumns::col_ustring_t    CharacterColumns::GetStatus()          { return GetField<col_ustring_t    >(f_status           ); }
CharacterColumns::col_pixbuf_t     CharacterColumns::GetArrowIcon()       { return GetField<col_pixbuf_t     >(f_arrow_icon       ); }
CharacterColumns::col_pixbuf_t     CharacterColumns::GetCharacterIcon()   { return GetField<col_pixbuf_t     >(f_character_icon   ); }
CharacterColumns::col_int_t        CharacterColumns::GetInit()            { return GetField<col_int_t        >(f_init             ); }
CharacterColumns::col_ustring_t    CharacterColumns::GetEffects()         { return GetField<col_ustring_t    >(f_effects          ); }
CharacterColumns::col_int_t        CharacterColumns::GetHitPoints()       { return GetField<col_int_t        >(f_hit_points       ); }
CharacterColumns::col_int_t        CharacterColumns::GetHpPercent()       { return GetField<col_int_t        >(f_hp_percent       ); }
CharacterColumns::col_bool_t       CharacterColumns::GetMonster()         { return GetField<col_bool_t       >(f_monster          ); }
CharacterColumns::col_int_t        CharacterColumns::GetPosition()        { return GetField<col_int_t        >(f_position         ); }
CharacterColumns::col_int_t        CharacterColumns::GetOrder()           { return GetField<col_int_t        >(f_order            ); }
CharacterColumns::col_character_t  CharacterColumns::GetCharacter()       { return GetField<col_character_t  >(f_character        ); }
CharacterColumns::col_connection_t CharacterColumns::GetConnection()      { return GetField<col_connection_t >(f_char_connection  ); }
CharacterColumns::col_bool_t       CharacterColumns::GetEditable()        { return GetField<col_bool_t       >(f_editable         ); }
CharacterColumns::col_bool_t       CharacterColumns::GetSensitive()       { return GetField<col_bool_t       >(f_sensitive        ); }
CharacterColumns::col_ustring_t    CharacterColumns::GetBackgroundColor() { return GetField<col_ustring_t    >(f_background_color ); }
CharacterColumns::col_ustring_t    CharacterColumns::GetForegroundColor() { return GetField<col_ustring_t    >(f_foreground_color ); }
CharacterColumns::col_weight_t     CharacterColumns::GetWeight()          { return GetField<col_weight_t     >(f_weight           ); }
CharacterColumns::col_style_t      CharacterColumns::GetStyle()           { return GetField<col_style_t      >(f_style            ); }
//
// Soft columns
//
CharacterColumns::col_int_t     & CharacterColumns::GetSoftCol             ( const mo_name_t& id) { return GetField<col_int_t     >(f_softCols[id].GetColumn          () ); }
CharacterColumns::col_ustring_t & CharacterColumns::GetSoftColString       ( const mo_name_t& id) { return GetField<col_ustring_t >(f_softCols[id].GetString          () ); }
CharacterColumns::col_weight_t  & CharacterColumns::GetSoftBold            ( const mo_name_t& id) { return GetField<col_weight_t  >(f_softCols[id].GetBold            () ); }
CharacterColumns::col_ustring_t & CharacterColumns::GetSoftBackgroundColor ( const mo_name_t& id) { return GetField<col_ustring_t >(f_softCols[id].GetBackgroundColor () ); }
CharacterColumns::col_ustring_t & CharacterColumns::GetSoftForegroundColor ( const mo_name_t& id) { return GetField<col_ustring_t >(f_softCols[id].GetForegroundColor () ); }

//===============================================================================
// View column lookups
//
CharacterColumns::column_t& CharacterColumns::GetNameCol()            { return *GetColumn(f_name)             .get(); }
CharacterColumns::column_t& CharacterColumns::GetPublicNameCol()      { return *GetColumn(f_publicName)       .get(); }
CharacterColumns::column_t& CharacterColumns::GetHpDisplayCol()       { return *GetColumn(f_hp_display)       .get(); }
CharacterColumns::column_t& CharacterColumns::GetStatusCol()          { return *GetColumn(f_status)           .get(); }
CharacterColumns::column_t& CharacterColumns::GetArrowIconCol()       { return *GetColumn(f_arrow_icon)       .get(); }
CharacterColumns::column_t& CharacterColumns::GetCharacterIconCol()   { return *GetColumn(f_character_icon)   .get(); }
CharacterColumns::column_t& CharacterColumns::GetInitCol()            { return *GetColumn(f_init)             .get(); }
CharacterColumns::column_t& CharacterColumns::GetEffectsCol()         { return *GetColumn(f_effects)          .get(); }
CharacterColumns::column_t& CharacterColumns::GetHitPointsCol()       { return *GetColumn(f_hit_points)       .get(); }
CharacterColumns::column_t& CharacterColumns::GetHpPercentCol()       { return *GetColumn(f_hp_percent)       .get(); }
CharacterColumns::column_t& CharacterColumns::GetMonsterCol()         { return *GetColumn(f_monster)          .get(); }
CharacterColumns::column_t& CharacterColumns::GetPositionCol()        { return *GetColumn(f_position)         .get(); }
CharacterColumns::column_t& CharacterColumns::GetOrderCol()           { return *GetColumn(f_order)            .get(); }
CharacterColumns::column_t& CharacterColumns::GetCharacterCol()       { return *GetColumn(f_character)        .get(); }
CharacterColumns::column_t& CharacterColumns::GetConnectionCol()      { return *GetColumn(f_char_connection)  .get(); }
CharacterColumns::column_t& CharacterColumns::GetEditableCol()        { return *GetColumn(f_editable)         .get(); }
CharacterColumns::column_t& CharacterColumns::GetSensitiveCol()       { return *GetColumn(f_sensitive)        .get(); }
CharacterColumns::column_t& CharacterColumns::GetBackgroundColorCol() { return *GetColumn(f_background_color) .get(); }
CharacterColumns::column_t& CharacterColumns::GetForegroundColorCol() { return *GetColumn(f_foreground_color) .get(); }
CharacterColumns::column_t& CharacterColumns::GetWeightCol()          { return *GetColumn(f_weight)           .get(); }
CharacterColumns::column_t& CharacterColumns::GetStyleCol()           { return *GetColumn(f_style)            .get(); }
//
CharacterColumns::column_t& CharacterColumns::GetSoftColCol             ( const molib::mo_name_t& id ) { return *GetColumn(f_softCols[id].GetColumn          ()) .get(); }
CharacterColumns::column_t& CharacterColumns::GetSoftColStringCol       ( const molib::mo_name_t& id ) { return *GetColumn(f_softCols[id].GetString          ()) .get(); }
CharacterColumns::column_t& CharacterColumns::GetSoftBoldCol            ( const molib::mo_name_t& id ) { return *GetColumn(f_softCols[id].GetBold            ()) .get(); }
CharacterColumns::column_t& CharacterColumns::GetSoftBackgroundColorCol ( const molib::mo_name_t& id ) { return *GetColumn(f_softCols[id].GetBackgroundColor ()) .get(); }
CharacterColumns::column_t& CharacterColumns::GetSoftForegroundColorCol ( const molib::mo_name_t& id ) { return *GetColumn(f_softCols[id].GetForegroundColor ()) .get(); }

}
// namespace Combatant

// vim: ts=4 sw=4 syntax=cpp.doxygen

