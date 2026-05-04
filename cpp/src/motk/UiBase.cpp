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




#include "UiBase.h"

#include <iostream>
#include <mo_string.h>

// LOCAL
//
#include "Common.h"

namespace motk
{

UIBase::UIBase()
{
	CreateUIManager();
}


UIBase::~UIBase()
{
	Clear();
}


void UIBase::CreateUIManager()
{
	f_refUIManager = Gtk::UIManager::create();
}


void UIBase::Clear()
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


void	UIBase::SetActionGroup( ActionGroupPtr val )
{
	f_actionGroup = val;
	f_refUIManager->insert_action_group( f_actionGroup );
}


MergeId	UIBase::AddUI( const QString& ui )
{
	try
	{
        f_mergeIds.push_back( f_refUIManager->add_ui_from_string( ui.toUtf8().data() ) );
	}
	catch( ... )
	{
#ifdef DEBUG
		std::cout << "Caught exception!" << std::endl;
#endif
	}

	return f_mergeIds.back();
}


void UIBase::RemoveUI( MergeId id )
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


void UIBase::ui( const QString& name, const QString& value )
{
	f_uiInfoMap[name] = value;
}


QString& UIBase::ui( const QString& name )
{
	return f_uiInfoMap[name];
}

String2String::iterator	UIBase::begin_ui()
{
	return f_uiInfoMap.begin();
}


String2String::iterator	UIBase::end_ui()
{
	return f_uiInfoMap.end();
}

AccelGroupPtr 	UIBase::GetAccelGroup()
{
	return f_refUIManager->get_accel_group();
}

Gtk::Widget* UIBase::GetWidget( const QString& name )
{
    return f_refUIManager->get_widget( name.toUtf8().data() );
}


QString UIBase::MakePath( const QString& path, const QString& name )
{
    molib::moWCString ret_name = QString(MENU_PREFIX + path + name);
	
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
// namespace motk
//
// vim: ts=8

