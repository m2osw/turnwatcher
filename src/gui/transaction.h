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

// C lib
#include <assert.h>

// STL
#include <stack>
#include <vector>
#include <memory>

// LOCAL
//
#include "common.h"

namespace Transactions
{

class Transaction
{
	public:
		typedef std::shared_ptr<Transaction>	    Pointer;
		typedef std::stack<Transaction::Pointer>	Stack;
		typedef std::vector<Transaction::Pointer>	List;

		Transaction()			{}
		virtual ~Transaction()	{}

		virtual void doit() = 0;
		virtual void undo() = 0;
};

class TransactionGroup : public Transaction
{
public:
		typedef std::shared_ptr<TransactionGroup>	Pointer;

		TransactionGroup( const char* name ) :
				f_name(name)
		{}
		virtual ~TransactionGroup() {}

		Glib::ustring name() const		{ return f_name; }
		bool empty() const				{ return f_list.empty(); }
		void clear()					{ f_list.clear(); }

		void addTransaction( Transaction::Pointer transact   );
		void addTransaction( TransactionGroup::Pointer group );
		void undo();
		void doit();

private:
		Glib::ustring		f_name;
		Transaction::List	f_list;
};


class TransactionManager
{
public:
		typedef std::shared_ptr<TransactionManager>	Pointer;

		TransactionManager();
		~TransactionManager();

		void addTransaction( Transaction::Pointer      transact, const bool doit = true );
		void addTransaction( TransactionGroup::Pointer transact, const bool doit = true );

		void undo();
		void redo();
		void clear();

		// Status
		//
		bool			isUndoListEmpty() const { return f_undoList.empty(); }
		bool			isRedoListEmpty() const { return f_redoList.empty(); }
		Glib::ustring	getUndoListTop() const;
		Glib::ustring	getRedoListTop() const;

		typedef sigc::signal<void>		VoidSignal;
		VoidSignal						signal_update()	{ return f_signalUpdate; }

private:
		// Non-copyable
		//
		//TransactionManager( const TransactionManager& ) {}
		//TransactionManager& operator =( const TransactionManager& ) {}

		// Data members
		//
		Transaction::Stack				f_undoList;
		Transaction::Stack				f_redoList;
		VoidSignal						f_signalUpdate;

		// Private methods
		//
		void updateStatus( const Glib::ustring& prefix, Transaction::Pointer transaction );
};

class TransactionGroupHelper
{
public:
		TransactionGroupHelper( const char* name, const bool auto_doit = true );
		virtual ~TransactionGroupHelper();

		void						AddTransaction( Transaction::Pointer transaction, const bool doit = false );
		TransactionGroup::Pointer	GetGroup() { return f_group; }
		void						Abort();

private:
		TransactionGroup::Pointer	f_group;
		bool						f_autoDoit;
};

}
// namespace Transactions


// vim: ts=4 sw=4 syntax=cpp.doxygen

