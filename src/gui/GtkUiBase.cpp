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




#include "GtkUiBase.h"
#include "Common.h"

#include <iostream>

namespace UI
{

GtkUiBase::GtkUiBase()
{
	CreateUIManager();
}


GtkUiBase::~GtkUiBase()
{
	Clear();
}


void GtkUiBase::CreateUIManager()
{
	f_refUIManager = Gtk::UIManager::create();
}


void GtkUiBase::Clear()
{
	f_uiInfoMap.clear();

	MergeIdVector::iterator	iter = f_mergeIds.begin();
	MergeIdVector::iterator	end  = f_mergeIds.end();

	for( ; iter != end; iter++ )
	{
		f_refUIManager->remove_ui( *iter);
	}

	f_mergeIds.clear();

	if( f_actionGroup )
	{
		f_refUIManager->remove_action_group( f_actionGroup );
		f_actionGroup.clear();
	}

	f_refUIManager.clear();
}


void	GtkUiBase::SetActionGroup( ActionGroupPtr val )
{
	f_actionGroup = val;
	f_refUIManager->insert_action_group( f_actionGroup );
}


MergeId	GtkUiBase::AddUI( const Glib::ustring& ui )
{
	try
	{
		f_mergeIds.push_back( f_refUIManager->add_ui_from_string( ui ) );
	}
	catch( ... )
	{
#ifdef DEBUG
		std::cout << "Caught exception!" << std::endl;
#endif
	}

	return f_mergeIds.back();
}


void GtkUiBase::RemoveUI( MergeId id )
{
	MergeIdVector::iterator	iter = f_mergeIds.begin();
	MergeIdVector::iterator	end  = f_mergeIds.end();

	for( ; iter != end; iter++ )
	{
		if( *iter == id )
		{
			f_refUIManager->remove_ui( id );
			f_mergeIds.erase( iter );
			break;
		}
	}
}


void GtkUiBase::ui( const Glib::ustring& name, const Glib::ustring& value )
{
	f_uiInfoMap[name] = value;
}


Glib::ustring& GtkUiBase::ui( const Glib::ustring& name )
{
	return f_uiInfoMap[name];
}

String2String::iterator	GtkUiBase::begin_ui()
{
	return f_uiInfoMap.begin();
}


String2String::iterator	GtkUiBase::end_ui()
{
	return f_uiInfoMap.end();
}

AccelGroupPtr	GtkUiBase::GetAccelGroup()
{
	return f_refUIManager->get_accel_group();
}

Gtk::Widget* GtkUiBase::GetWidget( const Glib::ustring& name )
{
	return f_refUIManager->get_widget( name );
}

Glib::ustring GtkUiBase::MakePath( const Glib::ustring& path, const Glib::ustring& name )
{
	QString ret_name = Glib::ustring(MENU_PREFIX + path + name).c_str();

	const int count = ret_name.Length();

	for( int idx = 0; idx < count; ++idx )
	{
		if( ret_name[idx] == '/' )
		{
			ret_name[idx]  = '|';
		}
	}

	return ret_name.c_str();
}

}
// namespace UI

// vim: ts=4 sw=4 noet syntax=cpp.doxygen

