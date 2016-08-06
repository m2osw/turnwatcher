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

// GTKMM
#include <gtkmm.h>
#include <gdkmm.h>

// Local includes
//
#include "base/character.h"
#include "base/CharacterColumns.h"
#include "base/stat.h"

namespace Combatant
{

class CharacterModel
{
public:
	// Public types
	//
	typedef std::weak_ptr<CharacterModel>      	 pointer_t;
	typedef Glib::RefPtr<Gtk::TreeStore>         tree_store_t;
	typedef Glib::RefPtr<Gdk::Pixbuf>            pixbuf_t;
	typedef Glib::RefPtr<Gtk::TreeModelSort>     tree_model_sort_t;
	typedef Gtk::TreeModel::iterator             iterator_t;
	typedef Gtk::TreeModel::Path                 path_t;
	typedef sigc::signal<void>                   void_signal_t;
	typedef sigc::signal1<void,path_t>           path_signal_t;
	typedef Character::pointer_t        		 character_t;
	typedef CharacterColumns::pointer_t	         columns_t;

	// Construction/destruction
	//
	static pointer_t Instance();
	static void Release();
	~CharacterModel();

    // Force the model to reload and refresh from CharacterManager. Also, emit signals
    //
    void Refresh();

	// Store
	//
	tree_store_t		getStore() const { return f_store; }

	// Contents access
	//
	iterator_t			findCharacter( character_t ch );
	void				init();
	void				clear();
	character_t			getCharacter( iterator_t iter );
	//
	path_t				getPath( iterator_t iter ) const { return f_store->get_path( iter ); }
	iterator_t			getIterator( path_t path ) const { return f_store->get_iter( path ); }
	path_t&				getCurrentPath()       { return f_currentPath; }
	const path_t&		getCurrentPath() const { return f_currentPath; }

	// Columns
	//
	columns_t			getColumns() { return f_columns; }

	// Sorting
	//
	void				setDefaultSort();
	int					getCurrentSort( Gtk::SortType& type );
	void				setCurrentSort( const int id, Gtk::SortType type );
	molib::mo_name_t	getSoftSort() const	{ return f_softSortType; }
	void				setSoftSort( const molib::mo_name_t type );
	path_t				getConvertedPosition( const int pos );

	// Navigating
	//
	void				next_tab_row( const bool forward );

	// Signals
	//
	void_signal_t		signal_soft_sort_changed()           { return f_signalSoftSortChanged; }
	void_signal_t		signal_model_changed()               { return f_signalModelChanged;    }
	void_signal_t		signal_model_reinited()              { return f_signalModelReinited;   }
	path_signal_t		signal_scroll_to_path( path_t path ) { return f_signalScrollToPath;    }

private:
	typedef std::shared_ptr<CharacterModel> private_pointer_t;
	static private_pointer_t f_instance;

	// Private data members
	//
	tree_store_t                           			f_store;
	tree_model_sort_t                      			f_initSort;
	path_t                                 			f_currentPath;
	CharacterColumns::pointer_t 					f_columns;
	pixbuf_t                               			f_pixbufArrow;
	pixbuf_t                               			f_pixbufMonster;
	pixbuf_t                               			f_pixbufCharacter;
	molib::mo_name_t                       			f_softSortType;
	void_signal_t                          			f_signalSoftSortChanged;
	void_signal_t                          			f_signalModelChanged;
	void_signal_t                          			f_signalModelReinited;
	path_signal_t                          			f_signalScrollToPath;

	// Singleton only
	//
    CharacterModel();
    CharacterModel( const CharacterModel& );
    CharacterModel& operator =( const CharacterModel& );

	// Private methods
	//
	void		insertCharacter( character_t ch );
	void 		removeCharacter( character_t ch );
	void		updateCharacter( character_t ch );
	//
	void		update();
	//void		updateAllCharacters();
	//
	void		internalClear      ( );
	void		updateSoftColumns  ( const Gtk::TreeRow& row, const Glib::ustring& default_background_color );
	void		updateRow          ( const Gtk::TreeRow& iter );
	void		unsetArrow         ( );
	void		setArrow           ( const int index );

	// Signals from model
	//
	void on_row_changed          ( const path_t& path, const iterator_t& iter );
	void on_row_deleted          ( const path_t& path );
	void on_row_has_child_toggled( const path_t& path, const iterator_t& iter );
	void on_row_inserted         ( const path_t& path, const iterator_t& iter );
	void on_rows_reordered       ( const path_t& path, const iterator_t& iter, int* new_order );

	// Other signals
	//
	void on_stats_changed();
};

}
// namespace Combatant

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
