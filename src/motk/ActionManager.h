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
#include <string>
#include <stack>

// GTKMM
//
#include <gtkmm.h>
#include <gdkmm.h>

// MOLIB
//
#include "mo/mo_base.h"
#include "mo/mo_name.h"
#include "mo/mo_event.h"
#include "mo/mo_props.h"
#include "mo/mo_string.h"

namespace motk
{

typedef Glib::RefPtr<Gdk::Pixbuf>			PixbufPtr;
typedef Glib::RefPtr<Gtk::Action>			ActionPtr;
typedef Glib::RefPtr<Gtk::ToggleAction>		ToggleActionPtr;
typedef Glib::RefPtr<Gtk::RadioAction>		RadioActionPtr;
typedef Glib::RefPtr<Gtk::ActionGroup>		ActionGroupPtr;
typedef std::map<QString,bool>   		String2Bool;
typedef std::stack<QString>			StringStack;

class ActionManager
{
public:
	ActionManager( Gtk::BuiltinIconSize defaultIconSize = Gtk::ICON_SIZE_LARGE_TOOLBAR );
	virtual ~ActionManager();

	struct ButtonImage : public molib::moBase
	{
	private:
		ButtonImage( const ButtonImage& );
		void operator =( ButtonImage& );

	public:
		enum ImageType { NONE, XPM, FILENAME };

		ImageType		f_type;
		const char* const*	f_xpmData;
		molib::moWCString	f_imageName;

		ButtonImage( const char* const* xpm_data ) :
			f_type(XPM),
			f_xpmData(xpm_data)	// Does not make a local copy of the buffer!
		{
		}

		ButtonImage( const molib::moWCString& str ) :
			f_type(FILENAME),
			f_xpmData(0),
			f_imageName(str)
		{
		}
	};

	typedef molib::moSmartPtr<ButtonImage>	ButtonImageSPtr;

protected:
	ActionGroupPtr			f_refActionGroup;

	void	SetDefaultIconSize( Gtk::BuiltinIconSize defaultIconSize );
	
    void	AddRadioAction(	const QString& name,
                            const QString& event,
							const ButtonImage* image_name = 0,
                            const QString& accel_key  = "" );

    void	AddToggleAction(const QString& name,
                            const QString& event,
							const ButtonImage* image,
                            const QString& accel_key );

    void	AddAction(		const QString& name,
                            const QString& event,
							const ButtonImage* image_name = 0,
                            const QString& accel_key  = "" );

    void	AddAction(		const QString& name,
                            const QString& event,
							const Gtk::StockID& stock_id,
                            const QString& accel_key = "" );

    void	AddMenu(		const QString& name,
                            const QString& event );

	motk::ActionPtr GetAction( const molib::moWCString& name );

    void	RemoveAction( const QString& event );
	void	RemoveAction( ActionPtr action );
	void	RemoveActions();
	void	CreateActionGroup();

    void	ActivateSignal( const QString& event, const bool activate = true );

private:
	Glib::RefPtr<Gtk::IconFactory>	f_iconFactory;
	Gtk::RadioButtonGroup			f_radioGroup;
	String2Bool						f_actionMap;

	struct ActionSignal
	{
		ActionPtr					f_action;
		sigc::connection			f_connection;
	};
	//
    typedef std::map<QString, ActionSignal> String2Signal;
	String2Signal					f_actionSignals;
	Gtk::BuiltinIconSize			f_defaultIconSize;

    virtual void OnButtonClicked( Glib::RefPtr<Gtk::Action>& action, const QString& event ) = 0;

    QString		GetImagePath( const molib::moWCString& imageName );
	PixbufPtr			GetPixbufImage( const char* const* xpm_data );
	PixbufPtr			GetPixbufImage( const molib::moWCString& imageName );
	Gtk::IconSource		CreateIconSource( const ButtonImage* image );
	void 				AddIconSource( const Gtk::StockID& stock_id, Gtk::IconSource icon_source );
    void 				AddStockItem( const QString& id,
                        const QString& label,
						const ButtonImage* image = 0 );
	void 				AddActionToGroup( ActionPtr action,
                        const QString& event,
                        const QString& accel_key = "" );
};

typedef molib::moSmartPtr<ActionManager>	ActionManagerSPtr;

}
// namespace motk


// vim: ts=4 sw=4 syntax=cpp.doxygen

