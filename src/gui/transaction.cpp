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




#include "transaction.h"
#include "SingletonManager.h"

namespace Transactions
{

//===============================================================================
// TransactionGroup
//===============================================================================

void TransactionGroup::addTransaction( Transaction::Pointer transact )
{
	f_list.push_back( transact );
}


void TransactionGroup::addTransaction( TransactionGroup::Pointer transact )
{
	Transaction::Pointer tr = std::dynamic_pointer_cast<Transaction>(transact);
	addTransaction( tr );
}


void TransactionGroup::undo()
{
	Transaction::List::reverse_iterator		iter = f_list.rbegin();
	Transaction::List::reverse_iterator		end  = f_list.rend();

	for( ; iter != end; ++iter )
	{
		(*iter)->undo();
	}
}

void TransactionGroup::doit()
{
	Transaction::List::iterator		iter = f_list.begin();
	Transaction::List::iterator		end  = f_list.end();

	for( ; iter != end; ++iter )
	{
		(*iter)->doit();
	}
}


//===============================================================================
// TransactionManager
//===============================================================================


TransactionManager::TransactionManager()
{
}


TransactionManager::~TransactionManager()
{
#ifdef DEBUG
	printf("TransactionManager::~TransactionManager()\n");
#endif
}


Glib::ustring TransactionManager::getUndoListTop() const
{
	Glib::ustring	name;
	//
	if( !isUndoListEmpty() )
	{
		Transaction::Pointer		top   = f_undoList.top();
		TransactionGroup::Pointer	group = std::dynamic_pointer_cast<TransactionGroup>(top);
		//
		if( group )
		{
			name = group->name();
		}
	}
	//
	return name;
}


Glib::ustring TransactionManager::getRedoListTop() const
{
	Glib::ustring	name;
	//
	if( !isRedoListEmpty() )
	{
		Transaction::Pointer		top   = f_redoList.top();
		TransactionGroup::Pointer	group = std::dynamic_pointer_cast<TransactionGroup>(top);
		//
		if( group )
		{
			name = group->name();
		}
	}
	//
	return name;
}


void TransactionManager::updateStatus( const Glib::ustring& prefix, Transaction::Pointer transaction )
{
	TransactionGroup::Pointer group = std::dynamic_pointer_cast<TransactionGroup>(transaction);
	if( group )
	{
		Glib::ustring msg;
		if( prefix != "" )
		{
			msg = prefix + " ";
		}
		msg += group->name();
		//
		auto appSettings( Application::Manager::Instance().lock()->GetAppSettings().lock() );
		assert(appSettings);
		//
		appSettings->StatusMsg( msg.c_str() );
		appSettings->signal_changed().emit();
	}
}


void TransactionManager::addTransaction( Transaction::Pointer transact, const bool doit )
{
	while( !f_redoList.empty() ) f_redoList.pop();
	f_undoList.push(transact);
	if( doit )
	{
		transact->doit();
	}
	f_signalUpdate.emit();
	updateStatus( "", transact );
}


void TransactionManager::addTransaction( TransactionGroup::Pointer transact, const bool doit )
{
	Transaction::Pointer tr( std::dynamic_pointer_cast<Transaction>(transact) );
	addTransaction( tr, doit );
}


void TransactionManager::undo()
{
	if( f_undoList.empty() ) return;
	Transaction::Pointer top( f_undoList.top() );
	top->undo();
	f_undoList.pop();
	f_redoList.push(top);
	updateStatus( gettext("_Undo"), top );
	f_signalUpdate.emit();
}

void TransactionManager::redo()
{
	if( f_redoList.empty() ) return;
	Transaction::Pointer top( f_redoList.top() );
	top->doit();
	f_redoList.pop();
	f_undoList.push(top);
	updateStatus( gettext("_Redo"), top );
	f_signalUpdate.emit();
}

void TransactionManager::clear()
{
	while( !f_redoList.empty() ) f_redoList.pop();
	while( !f_undoList.empty() ) f_undoList.pop();
	f_signalUpdate.emit();
}



//===============================================================================
// TransactionGroupHelper
//===============================================================================


TransactionGroupHelper::TransactionGroupHelper( const char* name, const bool auto_doit ) :
	f_group( new TransactionGroup( name ) ),
	f_autoDoit(auto_doit)
{
}


TransactionGroupHelper::~TransactionGroupHelper()
{
	if( !f_group->empty() )
	{
		TransactionManager::Pointer transMgr(Application::Manager::Instance().lock()->GetTransactionMgr().lock());
		assert(transMgr);
		transMgr->addTransaction( f_group, f_autoDoit );
	}
}


void TransactionGroupHelper::AddTransaction( Transaction::Pointer transaction, const bool doit )
{
	if( !f_autoDoit && doit )
	{
		transaction->doit();
	}
	//
	f_group->addTransaction( transaction );
}


void TransactionGroupHelper::Abort()
{
	f_group->clear();
}


}
// namespace Transactions

// vim: ts=4 sw=4 syntax=cpp.doxygen

