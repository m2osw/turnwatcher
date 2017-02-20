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



#ifndef MO_TRANSACTION_H
#define	MO_TRANSACTION_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_WCSTRING_H
#include	"mo_string.h"
#endif

#ifndef MO_NAME_H
#include	"mo_name.h"
#endif


namespace molib
{


class MO_DLL_EXPORT moTransactionManager;
typedef moSmartPtr<moTransactionManager>		moTransactionManagerSPtr;


class MO_DLL_EXPORT moTransaction : public moBase
{
public:
				moTransaction(const moWCString& name = "Transaction", const moWCString& message = "");
	virtual			~moTransaction();

	virtual const char *	moGetClassName(void) const;

	virtual void		DoIt(void) = 0;
	virtual void		Undo(void) = 0;

	virtual const moWCString& GetTransactionName(void) const;
	virtual const moWCString& GetTransactionMessage(void) const;

protected:
	moWCString		f_name;
	moWCString		f_message;

private:
	// prevent copies of transaction objects
				moTransaction(const moTransaction& transaction)
					: moBase(transaction)
				{
				}
	moTransaction&		operator = (const moTransaction& transaction) { return *this; }
};


typedef moSmartPtr<moTransaction>		moTransactionSPtr;
typedef moTmplList<moTransaction, moList>	moListOfTransactions;



class MO_DLL_EXPORT moTransactionGroup : public moTransaction
{
public:
				// no default name otherwise it hides the first transaction name!
				moTransactionGroup(const moWCString& name = "", const moWCString& message = "");
	virtual			~moTransactionGroup();

	virtual const char *	moGetClassName(void) const;

	virtual const moWCString& GetTransactionName(void) const;
	virtual const moWCString& GetTransactionMessage(void) const;

	void			Empty(void);
	bool			IsEmpty(void) const;

	virtual void		AddTransaction(moTransaction *transaction);
	virtual void		MergeTransactions(moListOfTransactions& transactions);

	// moTransaction
	virtual void		DoIt(void);
	virtual void		Undo(void);

private:
	moListOfTransactions	f_transactions;
};

typedef moSmartPtr<moTransactionGroup>		moTransactionGroupSPtr;




class MO_DLL_EXPORT moTransactionManager : public moBase
{
public:
	typedef	unsigned int	mark_id_t;

	// WARNING: the moTransactionCallback is not a managed object (not derived from moBase)
	class moTransactionCallback
	{
	public:
		virtual		~moTransactionCallback();

		virtual void	Transaction_Add(moTransactionManager *transaction_manager);
		virtual void	Transaction_Undo(moTransactionManager *transaction_manager);
		virtual void	Transaction_Redo(moTransactionManager *transaction_manager);

		typedef void	(moTransactionCallback::*callback_function_t)(moTransactionManager *transaction_manager);
	};

				moTransactionManager(const moWCString& name = "Transaction Manager");
	virtual const char *	moGetClassName(void) const;

	const moWCString&	GetName(void) const;

	void			Empty(void);
	bool			IsUndoEmpty(void) const;
	bool			IsRedoEmpty(void) const;

	mark_id_t		AddBeginMark(const moWCString& name = "Transaction", const moWCString& message = "");
	void			AddEndMark(mark_id_t id);
	void			AddTransaction(moTransaction *transaction);
	void			Undo(void);
	void			Redo(void);

	// the following returns a NULL pointer if the corresponding
	// list is empty
	moTransactionSPtr 	GetNextUndoTransaction(void) const;
	moTransactionSPtr 	GetNextRedoTransaction(void) const;

	void			RegisterCallback(moTransactionCallback *callback);
	void			UnregisterCallback(moTransactionCallback *callback);
	void			MakeCurrent(void);
	void			CallCallbacks(moTransactionCallback::callback_function_t func);

private:
	// prevent copies of the transaction manager objects
				moTransactionManager(const moTransactionManager& transaction_manager)
					: moBase(transaction_manager)
				{
				}
	moTransactionManager&	operator = (const moTransactionManager& transaction_manager) { return *this; }

	static const int	MAX_CALLBACKS = 5;

	moTransactionCallback *	f_callbacks[MAX_CALLBACKS];
	const moWCString	f_name;
	mutable moMutex		f_mutex;
	moListOfTransactions	f_undo;
	moListOfTransactions	f_redo;
	zbool_t			f_running;
	zuint32_t		f_mark;
};




// the following class is expected to be used on the stack to create a
// transaction group in such a way that it can be accepted (Commit)
// or cancelled (Rollback)
class MO_DLL_EXPORT moBuildTransactionGroup : public moBase
{
public:
				moBuildTransactionGroup(moTransactionGroup *group = 0);
	//virtual		~moBuildTransactionGroup();

	virtual const char *	moGetClassName(void) const;

	void			Empty(void);
	bool			IsEmpty(void) const;

	void			AddTransaction(moTransaction *transaction);

	void			Commit(moTransactionManager *transaction_manager);
	void			Rollback(void);

private:
	moTransactionGroupSPtr	f_group;
};

typedef moSmartPtr<moBuildTransactionGroup>	moBuildTransactionGroupSPtr;


// the following class is expected to be used on the stack to create a
// transaction group in such a way that it can be accepted (Commit)
// or cancelled (Rollback)
// this builder can be used when you do not always need to have a
// group (i.e. if you have a single transaction); it automatically
// decides whether a group is necessary; only draw back: you cannot
// make use of your own group
// this object is expected to be derived from to create transaction
// builders using the trans_functions tool
class MO_DLL_EXPORT moTransactionBuilder : public moBase
{
public:
				moTransactionBuilder(void);
	virtual			~moTransactionBuilder();

	virtual const char *	moGetClassName(void) const;

	void			Empty(void);
	bool			IsEmpty(void) const;

	void			AddTransaction(moTransaction *transaction);

	void			Commit(moTransactionManager *transaction_manager);
	void			Rollback(void);

private:
	zbool_t			f_done;
	moListOfTransactions	f_transactions;
};

typedef moSmartPtr<moTransactionBuilder>	moTransactionBuilderSPtr;




};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_TRANSACTION_H

