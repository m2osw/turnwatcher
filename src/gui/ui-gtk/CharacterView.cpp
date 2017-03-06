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

#include "CharacterView.h"

// molib
//
#include "mo/mo_name.h"

using namespace Application;
using namespace Attribute;
using namespace molib;

namespace UI
{

CharacterView::CharacterView()
	: f_isHUD               (false)
	, f_initializing        (true)
	, f_editingStarted      (false)
	, f_currentCol          (0)
	, f_nameColumn          (0)
	, f_publicNameColumn    (0)
	, f_typeColumn          (0)
#ifdef DEBUG
	, f_positionColumn      (0)
#endif
	, f_orderColumn         (0)
	, f_initColumn          (0)
	, f_hpColumn            (0)
    , f_statusColumn        (0)
    , f_effectsColumn       (0)
{
	set_rules_hint();
	set_headers_visible( true );
	set_headers_clickable( true );
	set_enable_search( false );

	auto appSettings( GetAppSettings().lock() );
	assert(appSettings);
	appSettings->signal_changed().connect( sigc::mem_fun( *this, &CharacterView::onAppSettingsChanged ) );
	//
	// Get the latest application settings
	//
	onAppSettingsChanged();

	f_selection = get_selection();
	f_selection->set_mode( Gtk::SELECTION_MULTIPLE );
	f_selection->signal_changed().connect( sigc::mem_fun( *this, &CharacterView::onSelectionChanged ) );

	// Listen for cursor changes
	//
	signal_cursor_changed().connect( sigc::mem_fun(*this, &CharacterView::on_cursor_changed) );

	// Set up StatManager signals
	//
	//GetStatMgr().lock()->signal_changed()     .connect( sigc::mem_fun(*this, &CharacterView::updateNewColumns ) );
	GetInitMgr().lock()->signal_start_rounds().connect( sigc::mem_fun(*this, &CharacterView::onStartRounds    ) );
	GetInitMgr().lock()->signal_end_rounds()  .connect( sigc::mem_fun(*this, &CharacterView::onEndRounds      ) );
	GetInitMgr().lock()->signal_turn_changed().connect( sigc::mem_fun(*this, &CharacterView::onTurnChanged    ) );

	// Connect to model
	//
    auto model(GetCharacterModel().lock());
    assert(model);
    //set_model( model->getStore() );
	//
	model->signal_soft_sort_changed().connect( sigc::mem_fun( *this, &CharacterView::updateNewColumns ) );
	model->signal_model_reinited()   .connect( sigc::mem_fun( *this, &CharacterView::onModelChange    ) );
	model->setDefaultSort();

	onModelChange();

    f_initializing = false;
}


CharacterView::~CharacterView()
{
	/* empty */
}


/// \brief Get the first selected character in the UI list and return the object.
//
CharacterView::character_pointer_t CharacterView::getFirstSelected()
{
	character_pointer_t ch;
	const selected_paths_t selected_rows( f_selection->get_selected_rows() );
	//
	if( selected_rows.size() > 0 )
	{
		auto model(GetCharacterModel().lock());
		assert(model);
		const Combatant::CharacterModel::path_t     selected_paths( *(selected_rows.begin()) );
		const Combatant::CharacterModel::iterator_t selected( model->getIterator( selected_paths ) );
		//
		ch = model->getCharacter( selected );
	}

	return ch;
}


/// \brief Find the selected characters in the UI list and return a list of the objects.
//
CharacterView::character_list_t CharacterView::getSelected()
{
	const selected_paths_t	rows( f_selection->get_selected_rows() );
	character_list_t		charList;

	for( auto path : rows )
	{
		auto model(GetCharacterModel().lock());
		assert(model);
		const Combatant::CharacterModel::iterator_t	list_model_iter( model->getIterator( path ) );
		const character_pointer_t			ch( model->getCharacter( list_model_iter ) );
		charList.push_back( ch );
	}

	return charList;
}


/// \brief Translate from a model iterator to a selection iterator, and select the line.
/// \note Unselectes current selection
//
void CharacterView::setSelected( const Combatant::CharacterModel::iterator_t iter )
{
	auto model(GetCharacterModel().lock());
	assert(model);
	f_selection->unselect_all();
	f_selection->select( model->getPath( iter ) );
}


/// \brief Look up a character and then selecte it.
//
void CharacterView::setSelected( character_pointer_t ch )
{
	auto model(GetCharacterModel().lock());
	assert(model);
	setSelected( model->findCharacter( ch ) );
} 


/// \brief Find a list of characters in the UI and select them all.
//
void CharacterView::setSelected( character_list_t chars )
{
	auto model(GetCharacterModel().lock());
	assert(model);

	for( auto ch : chars )
	{
		setSelected( model->findCharacter( ch ) );
	}
}


int CharacterView::addSoftColumn( Attribute::Stat::pointer_t stat )
{
	auto model(GetCharacterModel().lock());
	assert(model);
	const mo_name_t		         		 id     ( stat->id         () );
	const QString		             name   ( stat->name       () );
	Combatant::CharacterModel::columns_t columns( model->getColumns() );
	//
	Gtk::TreeView::Column* col = addTextColumn( name, columns->GetSoftColStringCol(id), columns->GetSoftColCol(id), false );
	Gtk::CellRendererText* renderer = dynamic_cast<Gtk::CellRendererText*>(col->get_first_cell_renderer());
	col->add_attribute( renderer->property_weight()         , columns->GetSoftBoldCol(id) );
	col->add_attribute( renderer->property_cell_background(), columns->GetSoftBackgroundColorCol(id) );
	col->add_attribute( renderer->property_foreground()     , columns->GetSoftForegroundColorCol(id) );
	if( stat->showOnToolbar() )
	{
		col->set_sort_column( columns->GetSoftColCol(id) );
	}
	else
	{
		col->set_sort_column( columns->GetSoftColStringCol(id) );
	}

	return col->get_sort_column_id();
}


void CharacterView::addSoftColumns( const bool onHud )
{
	typedef std::map<int,Attribute::Stat::pointer_t> StatMap;
	StatMap statMap;
	auto map( GetStatMgr().lock()->GetStats() );
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
				const int order = stat->order();
				statMap[order] = stat;
			}
		}
	}
	//
	for( auto statPair : statMap )
	{
		auto stat( statPair.second );
		const int order = stat->order();
		std::cerr << order << std::endl;
		f_softColumns[stat->id()] = addSoftColumn( stat );
	}
	
	// Add special roll column...
	//
	if( !onHud )
	{
		auto model(GetCharacterModel().lock());
		assert(model);
		Attribute::Stat::pointer_t stat = map[model->getSoftSort()];
		if( stat )
		{
			f_softColumns[stat->id()] = addSoftColumn( stat );
		}
	}
}


void CharacterView::addHUDSoftColumns()
{
	addSoftColumns( true /*onHud*/ );
}


void CharacterView::internalClear()
{
	remove_all_columns();
}


void CharacterView::AddDefaultColumns()
{
	internalClear();
	f_isHUD = false;

	// Create the first visible column (icon + name)
	//
	auto model(GetCharacterModel().lock());
	assert(model);
	Combatant::CharacterModel::columns_t	columns( model->getColumns( ) );
	if( GetInitMgr().lock()->InRounds() )
	{
		f_orderColumn	= addTextColumn( gettext("Order"),   columns->GetOrderCol() );
	}
	//
	f_nameColumn     = addNameColumn( gettext("Name") );
	f_typeColumn     = addIconColumn( gettext("Type"),       columns->GetCharacterIconCol() );
	f_initColumn     = addInitColumn( gettext("Init") );
#ifdef DEBUG
	f_positionColumn = addTextColumn( gettext("Position"),   columns->GetPositionCol() );
#endif
	f_hpColumn       = addTextColumn( gettext("Hit Points"), columns->GetHpDisplayCol(), columns->GetHitPointsCol() );
	f_statusColumn   = addTextColumn( gettext("Status"),     columns->GetStatusCol(),    columns->GetHpPercentCol() );
	f_effectsColumn  = addTextColumn( gettext("Effects"),    columns->GetEffectsCol() );

	addSoftColumns();
	//
	//model->setDefaultSort();

	// Notify change of font
	//
	onAppSettingsChanged();
}


void CharacterView::AddHUDColumns()
{
	internalClear();
	f_isHUD = true;

	// Create the first visible column (icon + name)
	//
	auto model(GetCharacterModel().lock());
	assert(model);
	Combatant::CharacterModel::columns_t	columns( model->getColumns( ) );
	if( GetInitMgr().lock()->InRounds() )
	{
		f_orderColumn	= addTextColumn( gettext("Order"),   columns->GetOrderCol() );
	}
	//
    f_nameColumn 		= addNameColumn( gettext("Name") );
    f_typeColumn 		= addIconColumn( gettext("Type")  ,  columns->GetCharacterIconCol() );
	f_statusColumn		= addTextColumn( gettext("Status"),  columns->GetStatusCol(), columns->GetHpPercentCol() );
	f_effectsColumn		= addTextColumn( gettext("Effects"), columns->GetEffectsCol() );
	
	addHUDSoftColumns();
	//
	//model->setDefaultSort();

	f_tooltips.set_tip( *this, gettext("Right click for more options") );

	// Notify change of font
	//
	onAppSettingsChanged();
}


void CharacterView::updateNewColumns()
{
	if( f_isHUD )
	{
		AddHUDColumns();
	}
	else
	{
		AddDefaultColumns();
	}
}


void CharacterView::onModelChange()
{
    auto model(GetCharacterModel().lock());
    assert(model);
    set_model( model->getStore() );
	//
	updateNewColumns();
}


void CharacterView::onSelectionChanged()
{
	if( !f_initializing && !f_editingStarted )
	{
#ifdef DEBUG
		printf( "CharacterView::onSelectionChanged(): emiting selection change!\n" );
#endif

		emitSelectionSignal();
	}
}


void CharacterView::onAppSettingsChanged()
{
	auto appSettings( GetAppSettings().lock() );
	assert(appSettings);
	//
	if( f_isHUD )
	{
		modify_font( Pango::FontDescription( appSettings->AltCombatantListFont() ) );
	}
	else
	{
		modify_font( Pango::FontDescription( appSettings->CombatantListFont() ) );
	}
	show_all_children();
}


void CharacterView::onStartRounds()
{
	updateNewColumns();
	onTurnChanged( 0 );
}


void CharacterView::onEndRounds()
{
	updateNewColumns();
}


void CharacterView::onTurnChanged( const int index )
{
	const auto path( GetCharacterModel().lock()->getConvertedPosition(index) );
    scroll_to_row( path );
	f_selection->unselect_all();
	f_selection->select( path );
}


Gtk::TreeView::Column* CharacterView::addNameColumn( const QString& name )
{
	// Add a special column with three sub-columns for the name
	//
	auto model(GetCharacterModel().lock());
	assert(model);

	Combatant::CharacterModel::columns_t	columns( model->getColumns( ) );
	Gtk::TreeView::Column* col = Gtk::manage( new Gtk::TreeView::Column( name ) );
	col->pack_start( columns->GetArrowIcon(), FALSE );

	// Use normal name for DM's view, public name for HUD view
	//
	const auto& name_col( f_isHUD? columns->GetPublicName(): columns->GetName() );
	col->pack_start( name_col, TRUE );

	// Sub-column 1--arrow slot
	//
	int rend_idx = 0;
	std::vector<Gtk::CellRenderer*> rends = col->get_cell_renderers();
	rends[rend_idx]->set_fixed_size(16, 16);
	col->add_attribute( rends[rend_idx++]->property_cell_background(), columns->GetBackgroundColorCol() );

	// Sub-column 2--name
	//
	Gtk::CellRendererText* name_rend = dynamic_cast<Gtk::CellRendererText*>(rends[rend_idx++]);
	col->clear_attributes(*name_rend);
	name_rend->property_xpad() = 5;
	name_rend->property_weight() = Pango::WEIGHT_HEAVY;
	col->add_attribute( name_rend->property_text()           , name_col );
	col->add_attribute( name_rend->property_cell_background(), columns->GetBackgroundColorCol() );
	col->add_attribute( name_rend->property_foreground()     , columns->GetForegroundColorCol() );
	col->add_attribute( name_rend->property_style()          , columns->GetStyleCol() );
	col->add_attribute( name_rend->property_sensitive()      , columns->GetSensitiveCol() );

	// General properties
	//
	col->set_expand( true );
	col->set_resizable( TRUE );
	col->set_alignment( 0.5 );
    col->set_sort_column( name_col );

	// Add the column to the tree-view (finally)
	//
	append_column( *col );

	return col;
}


Gtk::TreeView::Column* CharacterView::addInitColumn( const QString& name )
{
	auto model(GetCharacterModel().lock());
	assert(model);
	Combatant::CharacterModel::columns_t	columns( model->getColumns( ) );
	Gtk::CellRendererText* renderer = Gtk::manage( new Gtk::CellRendererText );
	Gtk::TreeView::Column* col = Gtk::manage( new Gtk::TreeView::Column( name, *renderer ) );
	renderer->signal_editing_started() .connect( sigc::mem_fun(*this, &CharacterView::on_editing_started ) );
	renderer->signal_editing_canceled().connect( sigc::mem_fun(*this, &CharacterView::on_editing_canceled) );
	col->add_attribute( renderer->property_text(),            columns->GetInitCol()            );
	col->add_attribute( renderer->property_cell_background(), columns->GetBackgroundColorCol() );
	col->add_attribute( renderer->property_foreground(),      columns->GetForegroundColorCol() );
	col->add_attribute( renderer->property_style(),           columns->GetStyleCol()           );
	col->add_attribute( renderer->property_sensitive(),       columns->GetSensitiveCol()       );
	col->add_attribute( renderer->property_editable(),        columns->GetEditableCol()        );
	col->set_resizable( false );
	col->set_alignment( 0.5 );
	col->set_sort_column( columns->GetInitCol() );
	append_column( *col );
	f_currentCol = col;
	return col;
}


Gtk::TreeView::Column* CharacterView::addIconColumn( const QString& name, Gtk::TreeModelColumnBase& column )
{
	auto model(GetCharacterModel().lock());
	assert(model);
	Combatant::CharacterModel::columns_t	columns( model->getColumns( ) );
	Gtk::CellRendererPixbuf* renderer = Gtk::manage( new Gtk::CellRendererPixbuf );
	//renderer->signal_editing_started() .connect( sigc::mem_fun(*this, &CharacterView::on_editing_started ) );
	//renderer->signal_editing_canceled().connect( sigc::mem_fun(*this, &CharacterView::on_editing_canceled) );
	Gtk::TreeView::Column* col = Gtk::manage( new Gtk::TreeView::Column( name, *renderer ) );
	renderer->set_fixed_size(32, 32);
	col->add_attribute( renderer->property_pixbuf(),          column                         );
	col->add_attribute( renderer->property_cell_background(), columns->GetBackgroundColorCol() );
	col->add_attribute( renderer->property_sensitive(),       columns->GetSensitiveCol()       );
	col->set_resizable( false );
	col->set_alignment( 0.5 );
	col->set_sort_column( columns->GetMonsterCol() );
	append_column( *col );
	return col;
}


Gtk::TreeView::Column* CharacterView::addTextColumn( const QString& name, Gtk::TreeModelColumnBase& column,
		Gtk::TreeModelColumnBase& sort_col, const bool render_colors )
{
	auto model(GetCharacterModel().lock());
	assert(model);

	Combatant::CharacterModel::columns_t	columns( model->getColumns( ) );
	Gtk::CellRendererText* renderer = Gtk::manage( new Gtk::CellRendererText );
	Gtk::TreeView::Column* col = Gtk::manage( new Gtk::TreeView::Column( name, *renderer ) );
	col->add_attribute( renderer->property_text(), column );
	if( render_colors )
	{
		col->add_attribute( renderer->property_cell_background(), columns->GetBackgroundColorCol() );
		col->add_attribute( renderer->property_foreground(),      columns->GetForegroundColorCol() );
	}
	col->add_attribute( renderer->property_style(),     columns->GetStyleCol()     );
	col->add_attribute( renderer->property_sensitive(), columns->GetSensitiveCol() );
	col->set_resizable( true );
	col->set_alignment( 0.5 );
	col->set_sort_column( sort_col );
	append_column( *col );
	return col;
}


Gtk::TreeView::Column* CharacterView::addTextColumn( const QString& name, Gtk::TreeModelColumnBase& column, const bool render_colors )
{
	return addTextColumn( name, column, column, render_colors );
}


Gtk::TreeView::Column* CharacterView::addToggleColumn( const QString& name, Gtk::TreeModelColumnBase& column )
{
	auto model(GetCharacterModel().lock());
	assert(model);
	Combatant::CharacterModel::columns_t	columns( model->getColumns( ) );
	Gtk::CellRendererToggle* renderer = Gtk::manage( new Gtk::CellRendererToggle );
	//renderer->signal_editing_started() .connect( sigc::mem_fun(*this, &CharacterView::on_editing_started ) );
	//renderer->signal_editing_canceled().connect( sigc::mem_fun(*this, &CharacterView::on_editing_canceled) );
	Gtk::TreeView::Column* col = Gtk::manage( new Gtk::TreeView::Column( name, *renderer ) );
	col->add_attribute( renderer->property_active(),          column                         );
	col->add_attribute( renderer->property_cell_background(), columns->GetBackgroundColorCol() );
	col->add_attribute( renderer->property_sensitive(),       columns->GetSensitiveCol()       );
	col->set_resizable( false );
	col->set_alignment( 0.5 );
	col->set_sort_column( column );
	append_column( *col );
	return col;
}


CharacterView::void_signal_t CharacterView::signal_double_click()
{
	return f_doubleClickSignal;
}


CharacterView::character_signal_t CharacterView::signal_selection_changed()
{
	return f_selectionChangedSignal;
}


void CharacterView::emitSelectionSignal()
{
	character_list_t chars( getSelected() );
	//
	if( !chars.empty() )
	{
		f_selectionChangedSignal.emit( chars );
	}
}


void CharacterView::on_cursor_changed()
{
#ifdef DEBUG
	std::cout << "on_cursor_changed()" << std::endl;
#endif
	if( !f_editingStarted )
	{
		auto model(GetCharacterModel().lock());
		assert(model);
		get_cursor( model->getCurrentPath(), f_currentCol );
	}
}


void CharacterView::on_editing_started( Gtk::CellEditable*, const QString& )
{
#ifdef DEBUG
	std::cout << "on_editing_started()" << std::endl;
#endif
	f_editingStarted = true;
}


void CharacterView::on_editing_canceled()
{
#ifdef DEBUG
	std::cout << "on_editing_canceled()" << std::endl;
#endif
	f_editingStarted = false;
}


void CharacterView::do_tab( const bool forward )
{
	auto model(GetCharacterModel().lock());
	assert(model);
	get_cursor( model->getCurrentPath(), f_currentCol );
	model->next_tab_row( forward );
	//
	set_cursor
		(
		 model->getCurrentPath(),
		 *f_currentCol,
		 true /* start_editing */
		);
}


bool CharacterView::on_key_press_event( GdkEventKey* event )
{
	auto model(GetCharacterModel().lock());
	assert(model);

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
					 model->getCurrentPath(),
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
						 model->getCurrentPath(),
						 *f_currentCol,
						 true/* start_editing */
						);
				}
				break;
		}
	}

	return return_value;
}


bool CharacterView::on_button_press_event( GdkEventButton* event )
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

