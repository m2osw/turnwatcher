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




#include "CharacterListUI.h"
#include "pixmaps/arrow.xpm"
#include "pixmaps/MnstrIcon.xpm"
#include "pixmaps/HeroIcon.xpm"

// molib
//
#include "mo/mo_string.h"

using namespace Application;
using namespace Attribute;

namespace UI
{

CharacterListUI::CharacterListUI() :
	f_isHUD		  	(false),
	f_initializing	(true),
	f_editingStarted(false),
	f_softSortType	(molib::moName("NONE")),
	f_dc			(-1),
	f_currentCol	(0)
{
	set_rules_hint();
	set_headers_visible( true );
	set_headers_clickable( true );
	set_enable_search( false );

	auto appSettings( GetAppSettings().lock() );
	assert(appSettings);
	appSettings->signal_changed().connect( sigc::mem_fun( *this, &CharacterListUI::onAppSettingsChanged ) );
	//
	// Get the latest application settings
	//
	onAppSettingsChanged();

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

	f_selection = get_selection();
	f_selection->set_mode( Gtk::SELECTION_MULTIPLE );
	f_selection->signal_changed().connect( sigc::mem_fun( *this, &CharacterListUI::onSelectionChanged ) );

	// Listen for cursor changes
	//
	signal_cursor_changed().connect( sigc::mem_fun(*this, &CharacterListUI::on_cursor_changed) );

	// Set up StatManager signals
	//
	GetStatMgr().lock()->signal_changed()     .connect( sigc::mem_fun(*this, &CharacterListUI::OnStatsChanged ) );
	GetInitMgr().lock()->signal_start_rounds().connect( sigc::mem_fun(*this, &CharacterListUI::OnStatsChanged ) );
	GetInitMgr().lock()->signal_end_rounds()  .connect( sigc::mem_fun(*this, &CharacterListUI::OnStatsChanged ) );

	// Initiative manager signals
	//
	connectInitMgrSignals();

	f_initializing = false;
}


CharacterListUI::~CharacterListUI()
{
	/* empty */
}


void CharacterListUI::connectInitMgrSignals()
{
	GetInitMgr().lock()->signal_start_rounds().connect( sigc::mem_fun(*this, &CharacterListUI::update   ) );
	GetInitMgr().lock()->signal_end_rounds()  .connect( sigc::mem_fun(*this, &CharacterListUI::update   ) );
	GetInitMgr().lock()->signal_turn_changed().connect( sigc::bind( sigc::mem_fun(*this, &CharacterListUI::setArrow ), true /* select */ ) );
}


void CharacterListUI::insertCharacter( Combatant::Character::Pointer ch )
{
	// Add the new entry into the store
	//
	Iterator iter = f_store->append();

	// Add the character to the column, and update the connection
	//
	Gtk::TreeRow row = *iter;
	row[*f_columns->GetCharacter()] = ch;
	sigc::connection connection = ch->signal_changed().connect(
			sigc::bind( sigc::mem_fun( *this, &CharacterListUI::updateCharacter ), ch ) );
	row[*f_columns->GetConnection()] = connection;

	// Update other detail about the character
	//
	updateRow( row );

	// Draw everything
	//  
	show_all();
}


void CharacterListUI::removeCharacter( Combatant::Character::Pointer ch )
{
	Iterator iter = findCharacter( ch );
	if( iter != f_store->children().end() )
	{
		// Disconnect the changed signal
		//
		Gtk::TreeRow row = *iter;
		sigc::connection connection = row[*f_columns->GetConnection()];
		connection.disconnect();

		// Erase from list
		//
		f_store->erase( iter );
	}
	//
	show_all();
}


void CharacterListUI::updateCharacter( Combatant::Character::Pointer ch )
{
	Iterator iter = findCharacter( ch );
	if( iter != f_store->children().end() )
	{
		updateRow( *iter );
	}
	//
	show_all();
}


void CharacterListUI::update()
{
	setArrow( GetInitMgr().lock()->CurrentInit(), true );
}


void CharacterListUI::updateAllCharacters()
{
#ifdef DEBUG
	printf( "CharacterListUI::updateAllCharacters()\n" );
#endif

	std::for_each( f_store->children().begin(), f_store->children().end(), [this]( const Gtk::TreeRow& row ) { updateRow(row); } );
	show_all();
}


void CharacterListUI::clear()
{
	if( f_store )
	{
		f_store->clear();
	}
}


// WARNING: Check for NULL return value if no selected made!
//
Combatant::Character::Pointer CharacterListUI::getFirstSelected()
{
	Gtk::TreeSelection::ListHandle_Path	rows 		= f_selection->get_selected_rows();
	Iterator							selected 	= f_store->children().end();

	if( rows.size() > 0 )
	{
		Gtk::TreeModel::Path path = *(rows.begin());
		selected = f_store->get_iter( path );
	}

	Combatant::Character::Pointer ch;

	if( selected != f_store->children().end() )
	{
		Gtk::TreeRow row = *selected;
		ch = row[*f_columns->GetCharacter()];
	}

	return ch;
}


Combatant::Character::List CharacterListUI::getSelected()
{
	SelectedPaths			rows = f_selection->get_selected_rows();
	Combatant::Character::List	charList;

	for( auto path : rows )
	{
		Iterator						list_model_iter	= f_store->get_iter( path );
		Gtk::TreeRow					row				= *(list_model_iter);
		Combatant::Character::Pointer	ch				= row[*(f_columns->GetCharacter())];
		charList.push_back( ch );
	}

	return charList;
}


void CharacterListUI::setSelected( Combatant::Character::Pointer ch )
{
	f_selection->unselect_all();
	//
	setSelected( findCharacter( ch ) );
}


void CharacterListUI::setSelected( Combatant::Character::List chars )
{
	f_selection->unselect_all();

	for( auto ch : chars )
	{
		setSelected( findCharacter( ch ) );
	}
}


void CharacterListUI::setSelected( Iterator iter )
{
	if( iter == f_store->children().end() )
	{
		return;
	}

	Gtk::TreeModel::Path path = f_store->get_path( iter );
	f_selection->select( path );
}


void CharacterListUI::OnStatsChanged()
{
	updateNewColumns();
}


int CharacterListUI::addSoftColumn( Attribute::Stat::Pointer stat )
{
	const molib::mo_name_t	id		= stat->id();
	const molib::moWCString	name	= stat->name();
	//
	Gtk::TreeView::Column* col = addTextColumn( name.c_str(), *f_columns->GetSoftColString(id), *f_columns->GetSoftCol(id), false );
	Gtk::CellRendererText* renderer = dynamic_cast<Gtk::CellRendererText*>(col->get_first_cell_renderer());
	col->add_attribute( renderer->property_weight()         , *f_columns->GetSoftBold(id) );
	col->add_attribute( renderer->property_cell_background(), *f_columns->GetSoftBackgroundColor(id) );
	col->add_attribute( renderer->property_foreground()     , *f_columns->GetSoftForegroundColor(id) );
	if( stat->showOnToolbar() )
	{
		col->set_sort_column( *f_columns->GetSoftCol(id) );
	}
	else
	{
		col->set_sort_column( *f_columns->GetSoftColString(id) );
	}

	return col->get_sort_column_id();
}


void CharacterListUI::addSoftColumns( const bool onHud )
{
	typedef std::map<int,Attribute::Stat::Pointer> StatMap;
	StatMap statMap;
	auto map = GetStatMgr().lock()->GetStats();
	//
	for( auto statPair : map )
	{
		auto stat( statPair.second );
		if( !stat->internal() && !stat->deleted() )
		{
			// We can only show the items that are *not* on the toolbar
			//
			bool show = false;
			if( onHud )
			{
				show = stat->showOnHUD() /*&& !stat->showOnToolbar()*/;
			}
			else
			{
				show = !stat->showOnToolbar();
			}
			//
			if( show )
			{
				statMap[stat->order()] = stat;
			}
		}
	}
	//
	for( auto statPair : statMap )
	{
		auto stat( statPair.second );
		f_softColumns[stat->id()] = addSoftColumn( stat );
	}
	
	// Add special roll column...
	//
	if( !onHud )
	{
		Attribute::Stat::Pointer stat = map[f_softSortType];
		if( stat )
		{
			f_softColumns[stat->id()] = addSoftColumn( stat );
		}
	}
}


void CharacterListUI::addHUDSoftColumns()
{
	addSoftColumns( true /*onHud*/ );
}


void CharacterListUI::AddDefaultColumns()
{
	internalClear();
	f_isHUD = false;
	f_columns.reset( new Combatant::CharacterColumns );

	// Use the main store
	//
	f_store		= Gtk::TreeStore::create    ( *(f_columns.get()) );
	f_initSort	= Gtk::TreeModelSort::create( f_store            );
	//
	set_model( f_store );

	// Create the first visible column (icon + name)
	//
	if( GetInitMgr().lock()->InRounds() )
	{
		f_orderColumn	= addTextColumn( gettext("Order"), *f_columns->GetOrder() );
	}
	//
	f_nameColumn 		= addNameColumn( gettext("Name") );
	f_typeColumn 		= addIconColumn( gettext("Type"), *f_columns->GetCharacterIcon() );
	f_initColumn		= addInitColumn( gettext("Init") );
#ifdef DEBUG
	f_positionColumn	= addTextColumn( gettext("Position"), *f_columns->GetPosition() );
#endif
	f_hpColumn			= addTextColumn( gettext("Hit Points"), *f_columns->GetHpDisplay(), *f_columns->GetHitPoints() );
	f_statusColumn		= addTextColumn( gettext("Status")	  , *f_columns->GetStatus()   , *f_columns->GetHpPercent() );
#if defined(WANT_EFFECTS)
	f_effectsColumn		= addTextColumn( gettext("Effects"), *f_columns->GetEffects() );
#endif
	
	addSoftColumns();
	//
	setDefaultSort();

	// Notify change of font
	//
	onAppSettingsChanged();
}


void CharacterListUI::AddHUDColumns()
{
	internalClear();

	f_isHUD		= true;
	f_columns.reset( new Combatant::CharacterColumns );

	// Use the main store
	//
	f_store		= Gtk::TreeStore::create    ( *(f_columns.get()) );
	f_initSort	= Gtk::TreeModelSort::create( f_store            );
	//
	set_model( f_store );

	// Create the first visible column (icon + name)
	//
	if( GetInitMgr().lock()->InRounds() )
	{
		f_orderColumn	= addTextColumn( gettext("Order"), *f_columns->GetOrder() );
	}
	//
	f_nameColumn 		= addNameColumn( gettext("Name") );
	f_typeColumn 		= addIconColumn( gettext("Type")  , *f_columns->GetCharacterIcon() );
		
	f_statusColumn		= addTextColumn( gettext("Status"), *f_columns->GetStatus()   , *f_columns->GetHpPercent() );
#if defined(WANT_EFFECTS)
	f_effectsColumn		= addTextColumn( gettext("Effects"), *f_columns->GetEffects() );
#endif
	
	addHUDSoftColumns();
	//
	setDefaultSort();

	f_tooltips.set_tip( *this, gettext("Right click for more options") );

	// Notify change of font
	//
	onAppSettingsChanged();
}


void CharacterListUI::updateNewColumns()
{
	// This clears out the internal model
	//
	if( f_isHUD )
	{
		AddHUDColumns();
	}
	else
	{
		AddDefaultColumns();
	}

	// Now, re-add the character entires from the character manager
	//
	for( auto ch : GetCharacterMgr().lock()->GetCharacters() )
	{
		insertCharacter( ch );
	}
}


void CharacterListUI::setDefaultSort()
{
	f_store->set_sort_column   ( *f_columns->GetOrder(), Gtk::SORT_ASCENDING );
	f_initSort->set_sort_column( *f_columns->GetOrder(), Gtk::SORT_ASCENDING );
}


int CharacterListUI::getCurrentSort( Gtk::SortType& type )
{
	int id;
	f_store->get_sort_column_id( id, type );
	return id;
}


void CharacterListUI::setCurrentSort( const int id, Gtk::SortType type )
{
	f_store->set_sort_column( id, type );
}


void CharacterListUI::on_sort_column_changed()
{
	updateAllCharacters();
}


void CharacterListUI::setDC( const int dc )
{
	f_dc = dc;
	updateAllCharacters();
}

void CharacterListUI::setSoftSort( const molib::mo_name_t type )
{
	f_softSortType = type;
	updateNewColumns();
	//
	if( f_softSortType != molib::moName("NONE") )
	{
		f_store->set_sort_column   ( *f_columns->GetSoftCol(f_softSortType), Gtk::SORT_DESCENDING );
		//f_initSort->set_sort_column( *f_columns->GetOrder(), Gtk::SORT_ASCENDING );
	}
}


void CharacterListUI::onSelectionChanged()
{
	if( !f_initializing && !f_editingStarted )
	{
#ifdef DEBUG
		printf( "CharacterListUI::onSelectionChanged(): emiting selection change!\n" );
#endif

		emitSelectionSignal();
	}
}


void CharacterListUI::onAppSettingsChanged()
{
	auto appSettings( GetAppSettings().lock() );
	assert(appSettings);
	//
	if( f_isHUD )
	{
		modify_font( Pango::FontDescription( appSettings->AltCombatantListFont().c_str() ) );
	}
	else
	{
		modify_font( Pango::FontDescription( appSettings->CombatantListFont().c_str() ) );
	}
	show_all_children();
}


Gtk::TreeView::Column* CharacterListUI::addNameColumn( const Glib::ustring& name )
{
	// Add a special column with three sub-columns for the name
	//
	Gtk::TreeView::Column* col = Gtk::manage( new Gtk::TreeView::Column( name ) );
	col->pack_start( *f_columns->GetArrowIcon(), FALSE );
	col->pack_start( *f_columns->GetName()     , TRUE  );
	//
	// Sub-column 1--arrow slot
	//
	int rend_idx = 0;
	std::vector<Gtk::CellRenderer*> rends = col->get_cell_renderers();
	rends[rend_idx]->set_fixed_size(16, 16);
	col->add_attribute( rends[rend_idx++]->property_cell_background(), *f_columns->GetBackgroundColor() );
	//
	// Sub-column 2--name
	//
	Gtk::CellRendererText* name_rend = dynamic_cast<Gtk::CellRendererText*>(rends[rend_idx++]);
	col->clear_attributes(*name_rend);
	name_rend->property_xpad() = 5;
	name_rend->property_weight() = Pango::WEIGHT_HEAVY;
	col->add_attribute( name_rend->property_text()           , *f_columns->GetName() );
	col->add_attribute( name_rend->property_cell_background(), *f_columns->GetBackgroundColor() );
	col->add_attribute( name_rend->property_foreground()     , *f_columns->GetForegroundColor() );
	col->add_attribute( name_rend->property_style()          , *f_columns->GetStyle() );
	col->add_attribute( name_rend->property_sensitive()      , *f_columns->GetSensitive() );
	//
	// General properties
	//
	col->set_expand( true );
	col->set_resizable( TRUE );
	col->set_alignment( 0.5 );
	col->set_sort_column( *f_columns->GetName() );
	//
	// Add the column to the tree-view (finally)
	//
	append_column( *col );
	return col;
}


Gtk::TreeView::Column* CharacterListUI::addInitColumn( const Glib::ustring& name )
{
	Gtk::CellRendererText* renderer = Gtk::manage( new Gtk::CellRendererText );
	Gtk::TreeView::Column* col = Gtk::manage( new Gtk::TreeView::Column( name, *renderer ) );
	renderer->signal_editing_started() .connect( sigc::mem_fun(*this, &CharacterListUI::on_editing_started ) );
	renderer->signal_editing_canceled().connect( sigc::mem_fun(*this, &CharacterListUI::on_editing_canceled) );
	col->add_attribute( renderer->property_text(), *f_columns->GetInit() );
	col->add_attribute( renderer->property_cell_background(), *f_columns->GetBackgroundColor() );
	col->add_attribute( renderer->property_foreground(), *f_columns->GetForegroundColor() );
	col->add_attribute( renderer->property_style(), *f_columns->GetStyle() );
	col->add_attribute( renderer->property_sensitive(), *f_columns->GetSensitive() );
	col->add_attribute( renderer->property_editable(), *f_columns->GetEditable() );
	col->set_resizable( false );
	col->set_alignment( 0.5 );
	col->set_sort_column( *f_columns->GetInit() );
	append_column( *col );
	f_currentCol = col;
	return col;
}


Gtk::TreeView::Column* CharacterListUI::addIconColumn( const Glib::ustring& name, Gtk::TreeModelColumnBase& column )
{
	Gtk::CellRendererPixbuf* renderer = Gtk::manage( new Gtk::CellRendererPixbuf );
	//renderer->signal_editing_started() .connect( sigc::mem_fun(*this, &CharacterListUI::on_editing_started ) );
	//renderer->signal_editing_canceled().connect( sigc::mem_fun(*this, &CharacterListUI::on_editing_canceled) );
	Gtk::TreeView::Column* col = Gtk::manage( new Gtk::TreeView::Column( name, *renderer ) );
	renderer->set_fixed_size(32, 32);
	col->add_attribute( renderer->property_pixbuf(), column );
	col->add_attribute( renderer->property_cell_background(), *f_columns->GetBackgroundColor() );
	col->add_attribute( renderer->property_sensitive(), *f_columns->GetSensitive() );
	col->set_resizable( false );
	col->set_alignment( 0.5 );
	col->set_sort_column( *f_columns->GetMonster() );
	append_column( *col );
	return col;
}


Gtk::TreeView::Column* CharacterListUI::addTextColumn( const Glib::ustring& name, Gtk::TreeModelColumnBase& column,
		Gtk::TreeModelColumnBase& sort_col, const bool render_colors )
{
	Gtk::CellRendererText* renderer = Gtk::manage( new Gtk::CellRendererText );
	Gtk::TreeView::Column* col = Gtk::manage( new Gtk::TreeView::Column( name, *renderer ) );
	col->add_attribute( renderer->property_text(), column );
	if( render_colors )
	{
		col->add_attribute( renderer->property_cell_background(), *f_columns->GetBackgroundColor() );
		col->add_attribute( renderer->property_foreground(), *f_columns->GetForegroundColor() );
	}
	col->add_attribute( renderer->property_style(), *f_columns->GetStyle() );
	col->add_attribute( renderer->property_sensitive(), *f_columns->GetSensitive() );
	col->set_resizable( true );
	col->set_alignment( 0.5 );
	col->set_sort_column( sort_col );
	append_column( *col );
	return col;
}


Gtk::TreeView::Column* CharacterListUI::addTextColumn( const Glib::ustring& name, Gtk::TreeModelColumnBase& column, const bool render_colors )
{
	return addTextColumn( name, column, column, render_colors );
}


Gtk::TreeView::Column* CharacterListUI::addToggleColumn( const Glib::ustring& name, Gtk::TreeModelColumnBase& column )
{
	Gtk::CellRendererToggle* renderer = Gtk::manage( new Gtk::CellRendererToggle );
	//renderer->signal_editing_started() .connect( sigc::mem_fun(*this, &CharacterListUI::on_editing_started ) );
	//renderer->signal_editing_canceled().connect( sigc::mem_fun(*this, &CharacterListUI::on_editing_canceled) );
	Gtk::TreeView::Column* col = Gtk::manage( new Gtk::TreeView::Column( name, *renderer ) );
	col->add_attribute( renderer->property_active(), column );
	col->add_attribute( renderer->property_cell_background(), *f_columns->GetBackgroundColor() );
	col->add_attribute( renderer->property_sensitive(), *f_columns->GetSensitive() );
	col->set_resizable( false );
	col->set_alignment( 0.5 );
	col->set_sort_column( column );
	append_column( *col );
	return col;
}


CharacterListUI::Iterator CharacterListUI::findCharacter( Combatant::Character::Pointer ch )
{
	auto iter      = f_store->children().begin();
	const auto end = f_store->children().end();
	for( ; iter != end; ++iter )
	{
		auto row( *iter );
		Combatant::Character::Pointer row_char( row[*f_columns->GetCharacter()] );
		if( row_char->name() == ch->name() )
		{
			break;
		}
	}

	return iter;
}


void CharacterListUI::UpdateSoftColumns( const Gtk::TreeRow& row, const Glib::ustring& default_background_color )
{
	Combatant::Character::Pointer	ch			( row[*f_columns->GetCharacter()]       );
	const Glib::ustring				fore_color	( row[*f_columns->GetForegroundColor()] );
	const Glib::ustring				back_color	( row[*f_columns->GetBackgroundColor()] );
	//
	auto map = GetStatMgr().lock()->GetStats();
	//
	for( auto statPair : map )
	{
		auto stat( statPair.second );
		auto id  ( stat->id() );
		const int  roll ( ch->getRoll( id ) );
		//
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
		
		if( show )
		{
			// Reset the custom colors for each row to default values
			//
			row[*f_columns->GetSoftCol(id)] 			= roll;
			row[*f_columns->GetSoftColString(id)]		= ss.str().c_str();
			row[*f_columns->GetSoftBold(id)] 			= Pango::WEIGHT_NORMAL;
			row[*f_columns->GetSoftForegroundColor(id)]	= fore_color;
			row[*f_columns->GetSoftBackgroundColor(id)]	= default_background_color;
	
			// If this is a custom stat and we are the same sort, then handle the coloration
			//
			if( stat->showOnToolbar() && (f_dc > -1) && (roll >= f_dc) )
			{
				row[*f_columns->GetSoftBold(id)] 			= Pango::WEIGHT_HEAVY;
				row[*f_columns->GetSoftForegroundColor(id)]	= "black";
				row[*f_columns->GetSoftBackgroundColor(id)]	= "green";
			}
		}
	}
}


void CharacterListUI::updateRow( const Gtk::TreeRow& row )
{
	bool							enabled = true;
	Pango::Style					style   = Pango::STYLE_NORMAL;
	Combatant::Character::Pointer	ch 	    = row[*f_columns->GetCharacter()];

	if( ch->monster() )
	{
		row[*f_columns->GetForegroundColor()] = "red";
		row[*f_columns->GetCharacterIcon  ()] = f_pixbufMonster;
		style = Pango::STYLE_ITALIC;
	}
	else
	{
		row[*f_columns->GetForegroundColor()] = "black";
		row[*f_columns->GetCharacterIcon  ()] = f_pixbufCharacter;
	}

	switch( ch->status()) 
	{
		case Combatant::Delayed:
			row[*f_columns->GetForegroundColor()] = "purple";
			break;
	
		case Combatant::Readied:
			row[*f_columns->GetForegroundColor()] = "orange";
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
			row[*f_columns->GetForegroundColor()] = "darkgrey";
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
	row[*f_columns->GetBackgroundColor()] = default_background_color;

	const int  init		= ch->getRoll( GetStatMgr().lock()->initId() );
	const bool editable	= false;

	row[*f_columns->GetStyle    ()] = style;
	row[*f_columns->GetSensitive()] = enabled;
	row[*f_columns->GetEditable ()] = editable;
	//
	if( f_isHUD )
	{
		row[*f_columns->GetName()] = ch->public_name().c_str();
	}
	else
	{
		row[*f_columns->GetName()] = ch->name().c_str();
	}
	row[*f_columns->GetInit()] = init;
	
	// Update DC if set and sorted
	//
	UpdateSoftColumns( row, default_background_color );
	
	const int size    = GetCharacterMgr().lock()->GetCharacters().size(); 
	const int pos     = ch->getPosition();
	const int mod_pos = size - pos;
	row[*f_columns->GetPosition()]	= pos;
	row[*f_columns->GetOrder()]		= mod_pos;

	const int BUFLEN = 64;
	char buffer[BUFLEN+1];
	const int hitpoints = ch->hitpoints() + ch->tempHP();
	snprintf( buffer, BUFLEN, "%d / %d", hitpoints, ch->maxHP() );
	row[*f_columns->GetHitPoints	()] = hitpoints;
	row[*f_columns->GetHpPercent	()] = ch->status_sort();
	row[*f_columns->GetHpDisplay	()] = buffer;
	row[*f_columns->GetMonster		()] = ch->monster();
	row[*f_columns->GetStatus		()] = ch->status_string();

#if defined(WANT_EFFECTS)
	Gtk::TreeModel::Children children = row.children();
	while( children.begin() != children.end() )	f_store->erase( children.begin() );

	// Count running effects
	//
	Effects::Effect::List effects;
	ch->getEffects( effects );
	//
	const int count = effects.size();
	int running_total = 0;
	int pending_total = 0;
	for( int i = 0; i < count; ++i )
	{
		Effects::Effect::Pointer fct( effects[i] );
		if( fct->startIn() > 0 )
		{
			++pending_total;
			++running_total;
		}
		else if( fct->roundsLeft() > 0 )
		{
			++running_total;
		}

#if 0
		// TODO: add proper effect row management
		//
		Gtk::TreeModel::Row	childrow = *(f_store->append( row.children() ) );
		childrow[*f_columns->GetName()]      = fct->name().c_str();
		childrow[*f_columns->GetSensitive()] = fct->roundsLeft() > 0;
#endif
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
	if( f_isHUD )
	{
		if( ch->monster() )
		{
			row[*f_columns->GetEffects()] = " ";
		}
		else
		{
			row[*f_columns->GetEffects()] = buffer;
		}
	}
	else
	{
		row[*f_columns->GetEffects()] = buffer;
	}
#endif
	setArrow( GetInitMgr().lock()->CurrentInit(), false /*select*/ );
}


void CharacterListUI::internalClear()
{
	Glib::RefPtr<Gtk::TreeStore>    empty_model;
	set_model(empty_model);
	f_store.clear();

	clear();
	remove_all_columns();
	f_initSort.clear();
	f_columns = 0;
	//f_softSaveColumns.clear();
	//f_softSkillColumns.clear();
}


void CharacterListUI::unsetArrow()
{
	for( const auto& row : f_initSort->children() )
	{
		row[*f_columns->GetArrowIcon()] = (PixbufPtr) 0;
	}
}


void CharacterListUI::setArrow( const int index, const bool select )
{
	// Turn off the old arrow
	//
	unsetArrow();

	// Santity checks
	//
	const size_t count = f_initSort->children().size();
	//
	if( !GetInitMgr().lock()->InRounds() || !count || index >= count )
	{
		return;
	}

	// Fix the arrow to the current row
	//
	Gtk::TreeModel::Row row = f_initSort->children()[index];
	row[*f_columns->GetArrowIcon()] = f_pixbufArrow;

	// Convert from an init-wise sorted iterator to a plain-store iterator
	// Then, convert it to the current sort-iterator (and then get the path)
	// A little confusing, but we can't scroll the row in question until
	// we've converted into the current-sort's way of ordering...
	//
	Iterator store_iter		= f_initSort->convert_iter_to_child_iter( f_initSort->children()[index] );
	Gtk::TreeModel::Path _path	= f_store->get_path( store_iter );
	scroll_to_row( _path );
	//
	// Cause the row to be selected
	//
	if( select )
	{
		f_selection->unselect_all();
		f_selection->select( _path );
	}
}


CharacterListUI::VoidSignal CharacterListUI::signal_double_click()
{
	return f_doubleClickSignal;
}


CharacterListUI::CharactersSignal CharacterListUI::signal_selection_changed()
{
	return f_selectionChangedSignal;
}


void CharacterListUI::emitSelectionSignal()
{
	Combatant::Character::List chars( getSelected() );
	//
	if( !chars.empty() )
	{
		f_selectionChangedSignal.emit( chars );
	}
}


void CharacterListUI::on_cursor_changed()
{
#ifdef DEBUG
	std::cout << "on_cursor_changed()" << std::endl;
#endif
	if( !f_editingStarted )
	{
		get_cursor( f_currentPath, f_currentCol );
	}
}


void CharacterListUI::on_editing_started( Gtk::CellEditable*, const Glib::ustring& )
{
#ifdef DEBUG
	std::cout << "on_editing_started()" << std::endl;
#endif
	f_editingStarted = true;
}


void CharacterListUI::on_editing_canceled()
{
#ifdef DEBUG
	std::cout << "on_editing_canceled()" << std::endl;
#endif
	f_editingStarted = false;
}


void CharacterListUI::next_tab_row( const bool forward )
{
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


void CharacterListUI::do_tab( const bool forward )
{
	get_cursor( f_currentPath, f_currentCol );
	next_tab_row( forward );
	//
	set_cursor
		(
		 f_currentPath,
		 *f_currentCol,
		 true /* start_editing */
		);
}


bool CharacterListUI::on_key_press_event( GdkEventKey* event )
{
	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	bool return_value = Gtk::Widget::on_key_press_event(event);

	if( !f_currentCol )
	{
		f_currentCol = get_column( 2 );
	}

	if( f_editingStarted )
	{
		switch( event->keyval )
		{
			case GDK_Tab:
				do_tab( true );
				break;

			case GDK_ISO_Left_Tab:
				do_tab( false );
				break;

			case GDK_Escape:
				set_cursor
					(
					 f_currentPath,
					 *f_currentCol,
					 false /* start_editing */
					);
				break;
		}
	}
	else
	{
		switch( event->keyval )
		{
			case GDK_Escape:
				{
					on_cursor_changed();
					set_cursor
						(
						 f_currentPath,
						 *f_currentCol,
						 true/* start_editing */
						);
				}
				break;
		}
	}

	return return_value;
}


bool CharacterListUI::on_button_press_event( GdkEventButton* event )
{
	bool retval = Gtk::TreeView::on_button_press_event( event );

	switch( event->button )
	{
	case 1:
		// Left mouse double-click
		//
		if( event->type == GDK_2BUTTON_PRESS )
		{
			f_doubleClickSignal.emit();
			retval = true;
		}
		break;
	
	case 3:
		// Right mouse click causes popup menu
		//
		if( f_isHUD )
		{
			Gtk::Menu* popupMenu = dynamic_cast<Gtk::Menu*>(GetActionsMgr().lock()->GetHUDMenu());
			//
			if( (event->type == GDK_BUTTON_PRESS) && popupMenu )
			{
				popupMenu->popup( event->button, event->time );
				//
				retval = true;
			}
		}
		break;
	}

	return retval;
}

}
// namespace UI


// vim: ts=4 sw=4 syntax=cpp.doxygen

