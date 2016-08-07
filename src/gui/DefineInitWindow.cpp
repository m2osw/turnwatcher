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
#include "DefineInitWindow.h"

#include <math.h>

using namespace motk;
using namespace Attribute;

namespace UI
{

namespace
{
    // Widget ids
    //
    const molib::moWCString	InitDiceInfo("InitDiceInfo");
}


DefineInitWindow::DefineInitWindow( Glib::ustring title, Gtk::Window& parent )
	: Gtk::Dialog( title, parent, true, true  )
{
	auto statMgr( GetStatMgr().lock() );
	assert( statMgr );
	f_initStat = statMgr->GetStat( statMgr->initId() );

	FillMainBox();
	//show_all_children();

	//
	// Lower buttons
	//
	if( alternative_button_order( Glib::RefPtr<const Gdk::Screen>(0) ) )
	{
		add_button( Gtk::Stock::OK      , Gtk::RESPONSE_OK     	);
		add_button( Gtk::Stock::CANCEL  , Gtk::RESPONSE_CANCEL 	);
	}
	else
	{
		add_button( Gtk::Stock::CANCEL  , Gtk::RESPONSE_CANCEL 	);
		add_button( Gtk::Stock::OK      , Gtk::RESPONSE_OK     	);
	}

	UpdateDlgFromInit();

	// StatManager
	//
	//statMgr->signal_changed().connect( sigc::mem_fun( *this, &DefineInitWindow::OnReloadTables ) );
}


DefineInitWindow::~DefineInitWindow()
{
	ClearMainBox();
}


void DefineInitWindow::FillMainBox()
{
	f_baseTable.AddItem( InitDiceInfo, gettext("Initiative Dice (default: 1d20)"), false, new Gtk::Entry, NULL );

	Gtk::VBox* vbox = get_vbox();
	vbox->pack_start( f_baseTable, true, true );

	show_all_children();
}


void DefineInitWindow::ClearMainBox()
{
	f_baseTable.Clear();
}


namespace
{
	Glib::ustring GetText( LabelTable& tbl, const molib::moName& id )
	{
		Gtk::Entry* e;
		tbl.GetItem( id, e );
		return e? e->get_text(): "1d20+0";
	}

	void SetText( LabelTable& tbl, const molib::moName& id, const Glib::ustring& text )
	{
		Gtk::Entry* e;
		tbl.GetItem( id, e );
		if( e ) e->set_text( text );
	}

#if 0
	molib::moWCString DiceFacesModifierToStr(int dice, int faces, int modifier)
	{
		if(dice <= 0) {
			dice = 1;
		}
		else if(dice > 10) {
			dice = 10;
		}
		if(faces < 2) {
			faces = 2;
		}
		else if(faces > 100) {
			faces = 100;
		}
		if(modifier < -1000) {
			modifier = -1000;
		}
		else if(modifier > 1000) {
			modifier = 1000;
		}
		if(modifier == 0) {
			return molib::moWCString::Format("%dd%d", dice, faces);
		}
		else {
			return molib::moWCString::Format("%dd%d%+d", dice, faces, modifier);
		}
	}

	void StrToDiceFacesModifier(const char *str, int& dice, int& faces, int& modifier)
	{
		// NOTE: we completely ignore garbage! it's up to the user to
		//       write a proper die reference
		int	sign;

		// reset values
		dice = 0;
		faces = 0;
		modifier = 0;

		while(isspace(*str)) {
			++str;
		}

		// parse 'dice'
		while(*str >= '0' && *str <= '9') {
			dice = dice * 10 + *str - '0';
			++str;
		}
		if(dice <= 0) {
			dice = 1;
		}
		else if(dice > 10) {
			dice = 10;
		}

		while(isspace(*str) || *str == 'd') {
			++str;
		}

		// parse 'faces'
		while(*str >= '0' && *str <= '9') {
			faces = faces * 10 + *str - '0';
			++str;
		}
		if(faces == 0) {
			// correct default is improperly defined!
			faces = 20;
		}
		else if(faces < 2) {
			faces = 2;
		}
		else if(faces > 100) {
			faces = 100;
		}

		while(isspace(*str)) {
			++str;
		}

		// parse 'modifier'
		if(*str == '+') {
			sign = 1;
		}
		else if(*str == '-') {
			sign = -1;
		}
		else {
			// no + or - thus no modifier
			return;
		}

		++str;	// skip the sign
		while(isspace(*str)) {
			++str;
		}

		while(*str >= '0' && *str <= '9') {
			modifier = modifier * 10 + *str - '0';
			++str;
		}
		if(modifier < 0) {
			modifier = 0;
		}
		else if(modifier > 1000) {
			modifier = 1000;
		}

		modifier *= sign;
	}
#endif
}




void DefineInitWindow::UpdateDlgFromInit()
{
	molib::moWCString dice_info(Common::DiceFacesModifierToStr(
			f_initStat->dice(), f_initStat->faces(), f_initStat->modifier()));
	SetText ( f_baseTable, InitDiceInfo, dice_info.c_str() );
}



#if !defined(DEMO_VERSION)
void DefineInitWindow::UpdateInitFromDlg()
{
// this does NOT work in the demo!
	int dice, faces, modifier;

	molib::moWCString dice_info( GetText( f_baseTable, InitDiceInfo ).c_str(), -1, molib::mowc::MO_ENCODING_UTF8 );
	Common::StrToDiceFacesModifier(dice_info.c_str(), dice, faces, modifier);
	f_initStat->dice     ( dice     );
	f_initStat->faces    ( faces    );
	f_initStat->modifier ( modifier );

	//f_changedSignal.emit( f_initStat );
}


void DefineInitWindow::GetData()
{
	UpdateInitFromDlg();
}
#endif


void DefineInitWindow::on_show()
{
#ifdef DEBUG
	std::cout << "DefineInitWindow::on_show()" << std::endl;
#endif

	Gtk::Widget* lastFocus = f_baseTable.GetLastFocus();
	if( lastFocus ) {
		set_focus( *lastFocus );
	}

	Gtk::Dialog::on_show();
}


void DefineInitWindow::on_hide()
{
#ifdef DEBUG
	std::cout << "DefineInitWindow::on_hide()" << std::endl;
#endif

	Gtk::Dialog::on_hide();
}


bool DefineInitWindow::on_key_press_event( GdkEventKey* event )
{
	// Call base class, to allow normal handling.
	bool return_value = Dialog::on_key_press_event(event);

	switch( event->keyval )
	{
	case GDK_Return:
		response( Gtk::RESPONSE_OK );
		break;

	case GDK_Escape:
		response( Gtk::RESPONSE_CANCEL );
		break;

	}

	return return_value;
}

}
// namespace UI

// vim: ts=8 sw=8

