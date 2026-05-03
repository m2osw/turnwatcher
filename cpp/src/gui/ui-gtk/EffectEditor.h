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

#pragma once

// STL
//
#include <map>

// MOLIB
//
#include "mo/mo_base.h"

// MOTK
//
#include "motk/LabelTable.h"

// GTKMM
//
#include <gtkmm.h>
#include <glibmm.h>

// LOCAL
//
#include "common.h"
#include "effect.h"
#include "TableConnections.h"

namespace UI
{

class EffectEditor
	: public Gtk::Dialog
{
public:
	EffectEditor( const QString& title );
	~EffectEditor();

	void SetEffect( Effects::Effect::pointer_t effect );
	bool Modified() const { return f_modified; }

private:
	TableConnections         f_tableConnections;
	AdjConnections           f_adjConnections;

	motk::LabelTable         f_table;
	Effects::Effect::pointer_t f_effect;
	bool                     f_modified;

	Gtk::Button*             f_cancelButton;
	Gtk::Button*             f_acceptButton;
	Gtk::Tooltips            f_tooltips;
	
	void                     FillMainBox();
	void                     InitTable();

	void                     ConnectAdjustments();
	void                     ConnectTables();
	void                     DisconnectAdjustments();
	void                     DisconnectTables();

	// Gtk::Dialog
	//
	virtual void		on_response( int response_id );

	// Gtk::Window
	//
	virtual bool		on_key_press_event( GdkEventKey* event );

	// motk::LabelTable
	//
	//void			OnSetFocus( motk::LabelTable* table );
	//void			OnToggled();
	void			OnChanged();

	void			OnCancelButton();
	void			OnAcceptButton();
};

}
// namespace UI


// vim: ts=8 sw=8

