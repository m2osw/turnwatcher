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

#include "CharacterModel.h"

// images
//
#include "pixmaps/arrow.xpm"
#include "pixmaps/MnstrIcon.xpm"
#include "pixmaps/HeroIcon.xpm"

// molib
//
#include "mo/mo_name.h"
#include "mo/mo_string.h"

// base
//
#include "base/CharacterManager.h"
#include "base/InitiativeManager.h"
#include "base/StatManager.h"

using namespace Application;
using namespace Attribute;
using namespace Combatant;
using namespace Initiative;

namespace Combatant
{


CharacterModel::private_pointer_t CharacterModel::f_instance;


CharacterModel::CharacterModel()
	: f_softSortType	(molib::moName("NONE"))
{
#ifdef INCLUDED_PIXMAPS
	f_pixbufArrow		= Gdk::Pixbuf::create_from_xpm_data( arrow_xpm );
	f_pixbufMonster		= Gdk::Pixbuf::create_from_xpm_data( MnstrIcon_xpm );
	f_pixbufCharacter	= Gdk::Pixbuf::create_from_xpm_data( HeroIcon_xpm );
#else
	f_pixbufArrow		= Gdk::Pixbuf::create_from_file( Common::GetFullPathnameForImage("arrow.xpm"    ).c_str() );
	f_pixbufMonster		= Gdk::Pixbuf::create_from_file( Common::GetFullPathnameForImage("MnstrIcon.xpm").c_str() );
	f_pixbufCharacter	= Gdk::Pixbuf::create_from_file( Common::GetFullPathnameForImage("HeroIcon.xpm" ).c_str() );
#endif

	f_pixbufMonster		= f_pixbufMonster->scale_simple( 32, 32, Gdk::INTERP_HYPER );
	f_pixbufCharacter	= f_pixbufCharacter->scale_simple( 32, 32, Gdk::INTERP_HYPER );

	// Set up StatManager signals
	//
	StatManager::Instance().lock()->signal_changed().connect( sigc::mem_fun(*this, &CharacterModel::on_stats_changed ) );

	// Initiative manager signals
	//
	InitiativeManager::Instance().lock()->signal_start_rounds().connect( sigc::mem_fun(*this, &CharacterModel::update   ) );
	InitiativeManager::Instance().lock()->signal_end_rounds()  .connect( sigc::mem_fun(*this, &CharacterModel::update   ) );
	InitiativeManager::Instance().lock()->signal_turn_changed().connect( sigc::mem_fun(*this, &CharacterModel::setArrow ) );

	// Character manager signals
	//
	CharacterManager::Instance().lock()->signal_character_added()    .connect( sigc::mem_fun( *this, &CharacterModel::insertCharacter ) );
	CharacterManager::Instance().lock()->signal_character_removed()  .connect( sigc::mem_fun( *this, &CharacterModel::removeCharacter ) );
	CharacterManager::Instance().lock()->signal_cleared()            .connect( sigc::mem_fun( *this, &CharacterModel::clear           ) );

    init();
}


CharacterModel::~CharacterModel()
{
	/* empty */
}


CharacterModel::pointer_t CharacterModel::Instance()
{
	if( !f_instance )
	{
        f_instance.reset( new CharacterModel );
	}
	return f_instance;
}


void CharacterModel::Release()
{
	f_instance.reset();
}


void CharacterModel::init()
{
    f_columns.reset( new Combatant::CharacterColumns );

	// Use the main store
	//
	f_store		= Gtk::TreeStore::create    ( *(f_columns.get()) );
	f_initSort	= Gtk::TreeModelSort::create( f_store            );

	// Hookup model signals
	//
	f_store->signal_row_changed          ( ).connect( sigc::mem_fun( *this, &CharacterModel::on_row_changed) );
	f_store->signal_row_inserted         ( ).connect( sigc::mem_fun( *this, &CharacterModel::on_row_inserted) );
	f_store->signal_row_has_child_toggled( ).connect( sigc::mem_fun( *this, &CharacterModel::on_row_has_child_toggled) );
	f_store->signal_row_deleted          ( ).connect( sigc::mem_fun( *this, &CharacterModel::on_row_deleted) );
	f_store->signal_rows_reordered       ( ).connect( sigc::mem_fun( *this, &CharacterModel::on_rows_reordered) );
}


void CharacterModel::clear()
{
	if( f_store )
	{
		f_store->clear();
	}
}


void CharacterModel::insertCharacter( Combatant::Character::pointer_t ch )
{
    assert(f_store);

	// Add the new entry into the store
	//
	const iterator_t iter( f_store->append() );

	// Add the character to the column, and update the connection
	//
	Gtk::TreeRow row( *iter );
	row[f_columns->GetCharacter()] = ch;
	sigc::connection connection = ch->signal_changed().connect( sigc::bind( sigc::mem_fun( *this, &CharacterModel::updateCharacter ), ch ) );
	row[f_columns->GetConnection()] = connection;

	// Update other detail about the character
	//
	updateRow( row );
}


void CharacterModel::removeCharacter( Combatant::Character::pointer_t ch )
{
    assert(f_store);

	const iterator_t iter( findCharacter( ch ) );
	if( iter != f_store->children().end() )
	{
		// Disconnect the changed signal
		//
		Gtk::TreeRow row( *iter );
		sigc::connection connection = row[f_columns->GetConnection()];
		connection.disconnect();

		// Erase from list
		//
		f_store->erase( iter );
	}
}


void CharacterModel::updateCharacter( Combatant::Character::pointer_t ch )
{
    assert(f_store);

    iterator_t iter( findCharacter( ch ) );
	if( iter != f_store->children().end() )
	{
		updateRow( *iter );
	}
}


#if 0
void CharacterModel::updateAllCharacters()
{
	std::for_each( f_store->children().begin(), f_store->children().end(), [this]( const Gtk::TreeRow& row ) { updateRow(row); } );
}
#endif


void CharacterModel::update()
{
	setArrow( InitiativeManager::Instance().lock()->CurrentInit() );
}


CharacterModel::character_t CharacterModel::getCharacter( iterator_t iter )
{
	return static_cast<character_t>( (*iter)[f_columns->GetCharacter()] );
}


void CharacterModel::next_tab_row( const bool forward )
{
    assert(f_store);

    auto children( f_store->children() );
	auto iter  ( f_store->get_iter( f_currentPath ) );
	auto begin ( children.begin() );
	auto end   ( children.end() );
	//assert(iter);

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

	f_currentPath = f_store->get_path( iter );
}

void CharacterModel::unsetArrow()
{
    const auto arrow_icon( f_columns->GetArrowIcon() );
    const auto& row_list( f_initSort->children() );
#if 0
    // TODO: this exhibits very strange behavior--it
    // only ever processes one row before just exiting the loop!
    // No compiler warnings, no errors, etc.! This
    // makes me worry about the stability of the new c++0x11
    // standard implementation in g++.
    //
    for( auto row : row_list )
	{
        row[arrow_icon] = (pixbuf_t) 0;
	}
#else
    const size_t count = row_list.size();
    for( size_t idx = 0; idx < count; ++idx )
    {
        auto row( row_list[idx] );
        row[arrow_icon] = (pixbuf_t) 0;
    }
#endif
}


void CharacterModel::setArrow( const int index )
{
    assert(f_store);

    // Turn off the old arrow
	//
	unsetArrow();

	// Santity checks
	//
	const size_t count = f_initSort->children().size();
	//
	if( !InitiativeManager::Instance().lock()->InRounds() || !count || index >= count )
	{
		return;
	}

	// Fix the arrow to the current row
	//
    Gtk::TreeModel::Row row( f_initSort->children()[index] );
	row[f_columns->GetArrowIcon()] = f_pixbufArrow;
}


CharacterModel::path_t CharacterModel::getConvertedPosition( const int pos )
{
    // Convert from an init-wise sorted iterator to a plain-store iterator
    // Then, convert it to the current sort-iterator (and then get the path)
    // A little confusing, but we can't scroll the row in question until
    // we've converted into the current-sort's way of ordering...
    //
    const iterator_t store_iter( f_initSort->convert_iter_to_child_iter( f_initSort->children()[pos] ) );
    return f_store->get_path( store_iter );
}


void CharacterModel::updateSoftColumns( const Gtk::TreeRow& row, const Glib::ustring& default_background_color )
{
	Combatant::Character::pointer_t	ch			( row[f_columns->GetCharacter()]       );
	const Glib::ustring				fore_color	( row[f_columns->GetForegroundColor()] );
	const Glib::ustring				back_color	( row[f_columns->GetBackgroundColor()] );
	//
	auto map( StatManager::Instance().lock()->GetStats() );
	//
	for( auto statPair : map )
	{
		auto stat( statPair.second );
		auto id  ( stat->id() );
		const int  roll ( ch->getRoll( id ) );
		std::stringstream	ss;
		ss << roll << std::ends;
		const Glib::ustring text( ss.str().c_str() );
#if 0
		bool show = true;
		//
		std::stringstream	ss;
		if( stat->showOnToolbar() )
		{
			ss << roll << std::ends;
			show = (f_softSortType == id);
		}
		else
		{
			const molib::moWCString	notes	= ch->getNotes( id );
			ss << notes.c_str() << std::ends;
		}
		//
#if 0
		// TODO: figure out how to hide monsters from the HUD
		//
		if( f_isHUD )
		{
			show = false;
			//
			if( stat->showOnHUD() )
			{
				if( ch->monster() )
				{
					show = stat->showMonsterOnHUD();
				}
				else
				{
					show = true;
				}
			}
		}
#endif
#endif
		
        if( !stat->internal() && !stat->deleted() )
        {
			// Reset the custom colors for each row to default values
			//
			row[f_columns->GetSoftCol(id)]				= roll;
			row[f_columns->GetSoftColString(id)]		= text;
			row[f_columns->GetSoftBold(id)]				= Pango::WEIGHT_NORMAL;
			row[f_columns->GetSoftForegroundColor(id)]	= fore_color;
			row[f_columns->GetSoftBackgroundColor(id)]	= default_background_color;
	
			// If this is a custom stat and we are the same sort, then handle the coloration
			//
			const int dc = AppSettings::Instance().lock()->LastDC();
			if( stat->showOnToolbar() && (dc > -1) && (roll >= dc) )
			{
				row[f_columns->GetSoftBold(id)]				= Pango::WEIGHT_HEAVY;
				row[f_columns->GetSoftForegroundColor(id)]	= "black";
				row[f_columns->GetSoftBackgroundColor(id)]	= "green";
			}
        }
	}
}


void CharacterModel::updateRow( const Gtk::TreeRow& row )
{
    assert(f_store);

    bool							enabled = true;
	Pango::Style					style   = Pango::STYLE_NORMAL;
	Combatant::Character::pointer_t	ch		= row[f_columns->GetCharacter()];

	if( ch->monster() )
	{
		row[f_columns->GetForegroundColor()] = "red";
		row[f_columns->GetCharacterIcon  ()] = f_pixbufMonster;
		style = Pango::STYLE_ITALIC;
	}
	else
	{
		row[f_columns->GetForegroundColor()] = "black";
		row[f_columns->GetCharacterIcon  ()] = f_pixbufCharacter;
	}

	switch( ch->status()) 
	{
		case Combatant::Delayed:
			row[f_columns->GetForegroundColor()] = "purple";
			break;
	
		case Combatant::Readied:
			row[f_columns->GetForegroundColor()] = "orange";
			break;

		default:
			break;
	}

	switch( ch->health() )
	{
		case Combatant::Dead:
			enabled = false;
		case Combatant::Disabled:
		case Combatant::Dying:
			row[f_columns->GetForegroundColor()] = "darkgrey";
			break;

		default:
			break;
	}

	Glib::ustring default_background_color = "white";
	//
	if( ch->getForcePosition() != -1 )
	{
		default_background_color = "cyan";
	}
	//
	row[f_columns->GetBackgroundColor()] = default_background_color;

	const int  init		= ch->getRoll( StatManager::Instance().lock()->initId() );
	const bool editable	= false;

	row[f_columns->GetStyle      ()] = style;
	row[f_columns->GetSensitive  ()] = enabled;
	row[f_columns->GetEditable   ()] = editable;
	row[f_columns->GetName       ()] = ch->name().c_str();
	row[f_columns->GetPublicName ()] = ch->public_name().c_str();
	row[f_columns->GetInit       ()] = init;

	// Update DC if set and sorted
	//
	updateSoftColumns( row, default_background_color );
	
	const int size    = CharacterManager::Instance().lock()->GetCharacters().size(); 
	const int pos     = ch->getPosition();
	const int mod_pos = size - pos;
	row[f_columns->GetPosition()]	= pos;
	row[f_columns->GetOrder()]		= mod_pos;

	const int BUFLEN = 64;
	char buffer[BUFLEN+1];
	const int hitpoints = ch->hitpoints() + ch->tempHP();
	snprintf( buffer, BUFLEN, "%d / %d", hitpoints, ch->maxHP() );
	row[f_columns->GetHitPoints	()] = hitpoints;
	row[f_columns->GetHpPercent	()] = ch->status_sort();
	row[f_columns->GetHpDisplay	()] = buffer;
	row[f_columns->GetMonster		()] = ch->monster();
	row[f_columns->GetStatus		()] = ch->status_string();

	Gtk::TreeModel::Children children = row.children();
	while( children.begin() != children.end() )	f_store->erase( children.begin() );

	// Count running effects
	//
	Effects::Effect::list_t effects;
	ch->getEffects( effects );
	//
	const int count = effects.size();
	int running_total = 0;
	int pending_total = 0;
	for( int i = 0; i < count; ++i )
	{
		Effects::Effect::pointer_t fct( effects[i] );
		if( fct->startIn() > 0 )
		{
			++pending_total;
			++running_total;
		}
		else if( fct->roundsLeft() > 0 )
		{
			++running_total;
		}
	}
	//
	if( count )
	{
		const char* fct_str = (count > 1)? "effects": "effect";
		if( pending_total )
		{
			snprintf( buffer, BUFLEN, "%d of %d %s, %d pending", running_total, count, fct_str, pending_total );
		}
		else
		{
			snprintf( buffer, BUFLEN, "%d of %d %s", running_total, count, fct_str );
		}
	}
	else
	{
		snprintf( buffer, BUFLEN, "%s", " " );
	}
	//
	row[f_columns->GetEffects()] = buffer;

	setArrow( InitiativeManager::Instance().lock()->CurrentInit() );
}


void CharacterModel::setDefaultSort()
{
    assert(f_store);
    f_store->set_sort_column   ( f_columns->GetOrder(), Gtk::SORT_ASCENDING );
	f_initSort->set_sort_column( f_columns->GetOrder(), Gtk::SORT_ASCENDING );
}


int CharacterModel::getCurrentSort( Gtk::SortType& type )
{
    assert(f_store);
    int id;
	f_store->get_sort_column_id( id, type );
	return id;
}


void CharacterModel::setCurrentSort( const int id, Gtk::SortType type )
{
    assert(f_store);
    f_store->set_sort_column( id, type );
}


void CharacterModel::setSoftSort( const molib::mo_name_t type )
{
    assert(f_store);
    f_softSortType = type;
	//
	f_signalSoftSortChanged.emit();
	//
	if( f_softSortType != molib::moName("NONE") )
	{
		f_store->set_sort_column( f_columns->GetSoftCol(f_softSortType), Gtk::SORT_DESCENDING );
	}
}


CharacterModel::iterator_t CharacterModel::findCharacter( Combatant::Character::pointer_t ch )
{
    assert(f_store);
    auto iter      = f_store->children().begin();
	const auto end = f_store->children().end();
	for( ; iter != end; ++iter )
	{
		auto row( *iter );
		Combatant::Character::pointer_t row_char( row[f_columns->GetCharacter()] );
		if( row_char->name() == ch->name() )
		{
			break;
		}
	}

	return iter;
}


void CharacterModel::on_row_changed( const path_t& path, const iterator_t& iter )
{
	f_signalModelChanged.emit();
}


void CharacterModel::on_row_deleted( const path_t& path )
{
	f_signalModelChanged.emit();
}


void CharacterModel::on_row_has_child_toggled( const path_t& path, const iterator_t& iter )
{
	f_signalModelChanged.emit();
}


void CharacterModel::on_row_inserted( const path_t& path, const iterator_t& iter )
{
	f_signalModelChanged.emit();
}


void CharacterModel::on_rows_reordered( const path_t& path, const iterator_t& iter, int* new_order )
{
	f_signalModelChanged.emit();
}


void CharacterModel::Refresh()
{
    on_stats_changed();
}


void CharacterModel::on_stats_changed()
{
    init();

	// Re-add the character entires from the character manager
	//
	for( auto ch : CharacterManager::Instance().lock()->GetCharacters() )
	{
		insertCharacter( ch );
	}

	f_signalModelReinited.emit();
}


}
// namespace Combatant

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
