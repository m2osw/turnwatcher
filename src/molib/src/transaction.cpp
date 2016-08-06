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



#ifdef MO_PRAGMA_INTERFACE
#pragma implementation "mo/mo_transaction.h"
#endif

#include	"mo/mo_transaction.h"

namespace molib
{

namespace
{
	// I suppose we could have some RAII header for such
	// classes?!
	class SetFlag
	{
	public:
		SetFlag(zbool_t& flag)
			: f_flag(flag)
		{
			if(f_flag) {
				// This is bad, you have some recursivity which is not supported
				// for transactions!
				assert(0);
				throw moError("You cannot add a transaction or call Undo() or Redo() recursively");
			}

			f_flag = true;
		}
		~SetFlag()
		{
			f_flag = false;
		}

	private:
		zbool_t&	f_flag;
	};

	class Mark : public moTransaction
	{
	public:
				Mark(moTransactionManager::mark_id_t id, const moWCString& name, const moWCString& message)
					: moTransaction(name, message),
					  f_id(id)
				{
				}

		moTransactionManager::mark_id_t GetID(void) const
				{
					return f_id;
				}

		// moTransaction
		virtual void	DoIt(void) { fprintf(stderr, "moTransaction: Mark: error: called DoIt() on a mark.\n"); assert(0); }
		virtual void	Undo(void) { fprintf(stderr, "moTransaction: Mark: error: called Undo() on a mark.\n"); assert(0); }

	private:
		moTransactionManager::mark_id_t f_id;
	};

	class BeginMark : public Mark
	{
	public:
				BeginMark(unsigned int id, const moWCString& name, const moWCString& message)
					: Mark(id, name, message)
				{
				}
	};

	typedef moSmartPtr<BeginMark>		BeginMarkSPtr;

	class EndMark : public Mark
	{
	public:
				EndMark(BeginMark *begin_mark)
					: Mark(begin_mark->GetID(), "End Mark", ""),
					  f_begin_mark(begin_mark)
				{
				}

		BeginMarkSPtr	GetBeginMark(void) const { return f_begin_mark; }

	private:
		BeginMarkSPtr	f_begin_mark;
	};
}



/************************************************************ DOC:

CLASS

	moTransaction

NAME

	Constructor -- initializes an moTransaction object
	moGetClassName -- return the name of this transaction class

SYNOPSIS

	moTransaction(const moWCString& name = "Transaction", const moWCString& message = "");
	virtual const char *moGetClassName(void) const;

	private:
	moTransaction(const moTransaction& transaction);
	moTransaction& operator = (const moTransaction& transaction);

PARAMETERS

	name -- the name of the transaction
	message -- the associated message of the transaction
	transaction -- another transaction

DESCRIPTION

	The constructor of a transaction requires you to supply
	a default name and a message for the transaction.

	Each time the DoIt() or Undo() functions are called,
	the user implemented functions are called with a pointer
	to the concern transaction manager.

	The moGetClassName() returns the name of this class.

	It is a good idea to overload this function in order to type
	your own transactions.

	The copy and assignment operators are both
	private so as to make sure that you cannot copy a
	transaction. Each transaction needs to be unique.

NOTES

	It is important to note that copying transactions would actually
	solve the problem of defining a transaction on the stack. In
	other words, the transaction manager could very easilly know
	whether it needs to copy the transaction (i.e. see the
	moBase::IsDynamicObject() function.)

	Yet, in most cases, that would be a waste of time since
	99.9% of the transactions will endup being added to the
	manager or a group and they would be copied.

RETURN VALUE

	The moGetClassName() function returns a pointer to a constant
	static string with the name of the class
	(molib::moBase::moTransaction).

SEE ALSO

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
moTransaction::moTransaction(const moWCString& name, const moWCString& message)
	: f_name(name),
	  f_message(message)
{
}


moTransaction::~moTransaction()
{
}


const char *moTransaction::moGetClassName(void) const
{
	return "molib::moBase::moTransaction";
}




/************************************************************ DOC:

CLASS

	moTransaction

NAME

	DoIt -- execute the transaction
	Undo -- cancel the transaction execution

SYNOPSIS

	virtual void DoIt(void) = 0;
	virtual void Undo(void) = 0;

DESCRIPTION

	The DoIt() function executes the transaction. This means
	calling the user DoIt() function. What is being done here
	must be undoable by the Undo() function.

	The Undo() function cancels what the DoIt() function has
	done. At times, this means copying back what was saved by
	the DoIt() function earlier.

SEE ALSO

	Constructor

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/




/************************************************************ DOC:

CLASS

	moTransaction

NAME

	GetTransactionName -- retrieve the name of this transaction
	GetTransactionMessage -- retrieve a message for this transaction

SYNOPSIS

	virtual const moWCString& GetTransactionName(void) const;
	virtual const moWCString& GetTransactionMessage(void) const;

DESCRIPTION

	The GetTransactionName() function retrieves the name of
	the transaction (i.e. "Add Control Point".) The name is
	often used for the Undo and Redo menu entries.

	The GetTransactionMessage() function retrieves a message
	to be displayed in a status bar whenever this transaction
	is executed or as a tooltip in the Undo/Redo menu entries.
	There is no default.

	The name and message of a transaction can be specified at
	the time the object is constructed. You can also overload
	the functions to dynamically generate the necessary
	information right when the function is called. Also, when
	you implement the transaction, you can assign the name
	to the f_name and message to the f_message variable members.

RETURN VALUE

	The GetTransactionName() function returns a string with the name.

	The GetTransactionMessage() function returns a string with the
	message.

SEE ALSO

	Constructor

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
const moWCString& moTransaction::GetTransactionName(void) const
{
	return f_name;
}


const moWCString& moTransaction::GetTransactionMessage(void) const
{
	return f_message;
}





/************************************************************ DOC:

CLASS

	moTransactionGroup

NAME

	Constructor -- initialize a transaction group
	moGetClassName -- return the name of this transaction class

SYNOPSIS

	moTransactionGroup(const moWCString& name, const moWCString& message);
	virtual const char *moGetClassName(void) const;

PARAMETERS

	name -- the name of this group
	message -- the message for this group

DESCRIPTION

	The constructor creates a new transaction which accepts one or more
	transactions, including a group transaction, as children. These
	are added using the AddTransaction() function.

	The name and message are the same as the name and message parameters
	as passed to the moTransaction constructor.

	The moGetClassName() returns the name of this class.

	The transaction group class is not usually overloaded unless you
	need to dynamically generate the name and message information.

	A transaction group object includes a set of transactions
	which all need to be executed at once whenever you want
	to Do, Undo and Redo a specific event.

NOTES

	A transaction group can include another transaction group. There are
	no limits.

RETURN VALUE

	The moGetClassName() function returns a pointer to a constant
	static string with the name of the class
	(molib::moBase::moTransaction::moTransactionGroup).

SEE ALSO

	AddTransaction()

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
moTransactionGroup::moTransactionGroup(const moWCString& name, const moWCString& message)
	: moTransaction(name, message)
{
	//f_transactions -- auto-init
}


moTransactionGroup::~moTransactionGroup()
{
}


const char *moTransactionGroup::moGetClassName(void) const
{
	return "molib::moBase::moTransaction::moTransactionGroup";
}



/************************************************************ DOC:

CLASS

	moTransactionGroup

NAME

	DoIt -- execute all the transactions in the group
	Undo -- cancel all the transactions in the group

SYNOPSIS

	virtual void DoIt(void);
	virtual void Undo(void);

PARAMETERS

	transaction_manager -- manager holding this transaction group

DESCRIPTION

	The DoIt() function goes through the entire list of transactions
	in the group and execute them one after another going forward
	(from the first to the last).

	The Undo() function goes through the entire list of transactions
	in the group and cancel them one after another going backward
	(from the last to the first).

BUGS

	A transaction group cannot be altered while executing its
	list of transactions (i.e. running the DoIt() or
	Undo() functions.) There are no safe guards to that effect
	at this time.

SEE ALSO

	GetTransactionName(), GetTransactionMessage(),
	Empty(), IsEmpty(), AddTransaction()

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
void moTransactionGroup::DoIt(void)
{
	moList::position_t	pos, max;

	// go forward
	max = f_transactions.Count();
	for(pos = 0; pos < max; ++pos) {
		f_transactions.Get(pos)->DoIt();
	}
}

void moTransactionGroup::Undo(void)
{
	moList::position_t	pos, max;

	// go backward
	max = f_transactions.Count();
	pos = max;
	while(pos > 0UL) {
		--pos;
		f_transactions.Get(pos)->Undo();
	}
}


/************************************************************ DOC:

CLASS

	moTransactionGroup

NAME

	GetTransactionName -- return the transaction group name
	GetTransactionMessage -- return the transaction group message

SYNOPSIS

	virtual const moWCString& GetTransactionName(void) const;
	virtual const moWCString& GetTransactionMessage(void) const;

DESCRIPTION

	The GetTransactionName() and GetTransactionMessage() functions
	return the first which is not an empty string:

	. The name/message of this transaction group if it is not empty;

	. When there is at least one transaction, the name/message of
	  the very first transaction in the group;

	. An empty string if nothing else returned a non-empty string.

NOTES

	In general, the name of a transaction is used for the corresponding
	Undo and Redo menu entries.

	The message of a transaction is used in the status bar and eventually
	the tooltip of the Undo and Redo menu entries.

	This function does not do any internationalization. It is your
	responsibility.

RETURN VALUE

	GetTransactionName() the transaction name for this group

	GetTransactionMessage() the transaction message for this group

SEE ALSO

	Constructor

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
const moWCString& moTransactionGroup::GetTransactionName(void) const
{
	if(!f_name.IsEmpty()) {
		return f_name;
	}

	if(f_transactions.IsEmpty()) {
		return g_empty_string;
	}

	return f_transactions.Get(0)->GetTransactionName();
}


const moWCString& moTransactionGroup::GetTransactionMessage(void) const
{
	if(!f_message.IsEmpty()) {
		return f_message;
	}

	if(f_transactions.IsEmpty()) {
		return g_empty_string;
	}

	return f_transactions.Get(0)->GetTransactionMessage();
}







/************************************************************ DOC:

CLASS

	moTransactionGroup

NAME

	Empty -- clear all the transactions
	IsEmpty -- check whether this group has any sub-transactions
	AddTransaction -- add a sub-transaction

SYNOPSIS

	void Empty(void);
	bool IsEmpty(void) const;
	virtual void AddTransaction(moTransaction *transaction);

PARAMETERS

	transaction -- the transaction being added

DESCRIPTION

	The Empty() function removes all the transactions previously
	added to a transaction group.

	The IsEmpty() function returns true if no transaction was
	added to a transaction group.

	The AddTransaction() function adds the specified transaction
	to the group. All the newly added transactions are added at
	the end of the current transaction list.

	The function is virtual so one can derive from this group
	and catch the event as required. You should always call
	the default AddTransaction() function to ensure that the
	transactions are added to the group, but you can prevent
	some transactions from being added in this way.

SEE ALSO

	DoIt(), Undo()

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
void moTransactionGroup::Empty(void)
{
	f_transactions.Empty();
}


bool moTransactionGroup::IsEmpty(void) const
{
	return f_transactions.IsEmpty();
}


void moTransactionGroup::AddTransaction(moTransaction *transaction)
{
	if(transaction != 0) {
		f_transactions.Insert(transaction);
	}
}


void moTransactionGroup::MergeTransactions(moListOfTransactions& transactions)
{
	f_transactions += transactions;
}




/************************************************************ DOC:

CLASS

	moTransactionManager

NAME

	Constructor -- private initializer
	moGetClassName -- return the name of this class
	GetName -- get the name of this transaction manager

SYNOPSIS

	moTransactionManager(const moWCString& name);
	virtual const char *moGetClassName(void) const;
	const moWCString& GetName(void) const;

PARAMETERS

	name -- the name to use for this transaction manager

DESCRIPTION

	The constructor initializes a transaction manager with
	defaults. It will be empty (nothing to Redo or Undo.)
	The name is any identifier you deem useful for your
	implementation. In general, if you have only one
	transaction manager, you can set it to the name of your
	application.

	Often, the transaction manager name will be used in the
	menu to tell the end user what is about to be done or
	undone. For instance, in Snap! we have a "Shape Editor"
	transaction manager and you can add a "Line" to your
	shape. The Undo text can thus be something like this:

		"Shape Editor: Undo Line"

	The moGetClassName() function returns the name of this class.

	The GetName() function returns the name passed to the
	transaction manager on construction.

NOTES

	The transaction manager is multi-thread safe.

	There is no SetName() function at this time. It does not
	seem to be sensical to be able to change the name of
	a transaction manager on the fly.

RETURN

	moGetClassName() returns "molib::moBase::moTransactionManager"

	GetName() returns the name of the transaction manager

SEE ALSO

	AddTransaction(), Undo(), Redo(), RegisterCallback()

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
moTransactionManager::moTransactionManager(const moWCString& name)
	: f_name(name)
{
	int	i;

	for(i = 0; i < MAX_CALLBACKS; ++i) {
		f_callbacks[i] = 0;
	}

	//f_mutex -- auto-init
	//f_undo -- auto-init
	//f_redo -- auto-init
	//f_running -- auto-init
	//f_mark -- auto-init
}


const char *moTransactionManager::moGetClassName(void) const
{
	return "molib::moBase::moTransactionManager";
}


const moWCString& moTransactionManager::GetName(void) const
{
	return f_name;
}







/************************************************************ DOC:

CLASS

	moTransactionManager

NAME

	Empty -- reset the transaction manager
	IsUndoEmpty -- check whether the undo stack is empty
	IsRedoEmpty -- check whether the redo stack is empty

SYNOPSIS

	void Empty(void);
	bool IsUndoEmpty(void) const;
	bool IsRedoEmpty(void) const;

DESCRIPTION

	The Empty() function clears the undo and redo stacks.

	The IsUndoEmpty() function returns true if there are no
	transactions to undo in the current stack.

	The IsRedoEmpty() function returns true if there are no
	transactions to redo in the current stack.

	In general, you want to use the IsUndoEmpty() and
	IsRedoEmpty() function to know whether you need to
	grey out the Undo and Redo menu entries.

NOTES

	The transaction manager is multi-thread safe.

RETURN

	IsUndoEmpty() returns true when there is nothing to undo

	IsRedoEmpty() returns true when there is nothing to redo

SEE ALSO

	GetName()

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
void moTransactionManager::Empty(void)
{
	moLockMutex lock(f_mutex);
	f_undo.Empty();
	f_redo.Empty();
}


bool moTransactionManager::IsUndoEmpty(void) const
{
	moLockMutex lock(f_mutex);
	moList::position_t idx = f_undo.Count();
	for(;;) {
		if(idx == 0UL) {
			return true;
		}
		if(dynamic_cast<Mark *>(f_redo.Get(idx)) == 0) {
			// not a mark thus not an empty list
			return false;
		}
		--idx;
	}
	/*NOTREACHED*/
}


bool moTransactionManager::IsRedoEmpty(void) const
{
	moLockMutex lock(f_mutex);
	moList::position_t idx = f_redo.Count();
	for(;;) {
		if(idx == 0UL) {
			return true;
		}
		if(dynamic_cast<Mark *>(f_redo.Get(idx)) == 0) {
			// not a mark thus not an empty list
			return false;
		}
		--idx;
	}
	/*NOTREACHED*/
}






/************************************************************ DOC:

CLASS

	moTransactionManager

NAME

	AddTransaction -- add a transaction to the manager
	AddBeginMark -- create a mark
	AddEndMark -- close a mark
	Undo -- undo one transaction
	Redo -- redo one transaction

SYNOPSIS

	void AddTransaction(moTransaction *transaction);
	mark_id_t AddBeginMark(const moWCString& name, const moWCString& message);
	void AddEndMark(mark_id_t id);
	void Undo(void);
	void Redo(void);

PARAMETERS

	transaction -- the transaction to add or NULL
	name -- the name of this mark
	message -- the message for this mark
	id -- the identifier returned by AddBeginMark()

DESCRIPTION

	The AddTransaction() function adds one transaction to
	the Undo stack and executes the transaction. Later,
	you can cancel the transaction by calling Undo().

	Note that the AddTransaction() function has the side
	effect of clearing the redo stack.

	The transaction will already be stacked when the
	DoIt() function is called.

	The same transaction can be added as many times as
	necessary to any number of transaction managers.
	As long as DoIt() and Undo() have no side effects
	on the transaction object that is.

	The AddBeginMark() and AddEndMark() functions create
	mark transactions. The AddBeginMark() assigns a
	unique identifier to the begin mark. You must specify
	this unique identifier at the time you create the
	end mark with a call to AddEndMark().

	The begin and end marks must match. You can create any
	number of begin marks, but you must close them in the
	opposite order you began them (open 1, 2, 3, then close
	3, 2, 1). In debug, the AddEndMark() function writes an
	error message and asserts.

	The marks are used to create large groups of
	transactions which span on multiple set of
	transactions.

	Say, for instance, that a user wants to draw a line.
	Each click represents a point. As the line is being
	drawn, the user certainly wants to be able to undo
	the last few points and not the entire drawing.
	However, once the line is done, selecting Undo should
	most certainly undo the entire object and not the
	last point of the object. Marks let you do that.
	At the beginning of a line, add a "begin line" mark,
	and the end of the line, add a "end line" mark.
	The Undo/Redo will do exactly what you expect!

	The Undo() function cancels one transaction. First,
	the transaction is moved from the Undo to the Redo
	stack. Then it calls the Undo() function of the
	transaction.

	The Redo() function re-execute one transaction. First,
	the transaction is moved from the Redo to the
	Undo stack. Then it calls the DoIt() function of
	the transaction.

	If you installed callbacks on the transaction
	manager, then they get called right before any
	one of these functions is complete.

	The AddTransaction() calls the Transaction_Add() event.

	The Undo() calls the Transaction_Undo() event.

	The Redo() calls the Transaction_Redo() event.

NOTES

	If you pass a null pointer to AddTransaction(), the
	function returns right away.

	The AddBeginMark() and AddEndMark() create a special
	internal transaction of type mark.

	The AddBeginMark() and AddEndMark() generate a call
	to the Transaction_Add() event. This is sensical since
	marks are transactions and adding a transaction to
	your transaction manager will in general result in
	a new message in the Edit Menu and Status Bar.

	If you call an AddBeginMark() and somehow you endup
	not needing it, you can call AddEndMark() to delete
	it since a begin mark right away followed by an end
	mark is removed from the list.

	The Undo() and Redo() functions do nothing if there
	isn't a transaction to cancel or re-execute.

	The transactions are done with a locked mutex so
	other threads cannot access the transaction manager
	while a transaction is being performed.

BUGS

	Calling the DoIt() or Undo() functions on a transaction
	should *never* generate a call to AddTransaction().

	The DoIt() and Undo() functions cannot be called
	recursively.

	The Transaction_Add() event should probably not be
	called when an AddBeginMark() is called.

SEE ALSO

	GetNextUndoTransaction(), GetNextRedoTransaction(),
	RegisterCallback(), UnregisterCallback(),
	CallCallbacks()

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
moTransactionManager::mark_id_t moTransactionManager::AddBeginMark(const moWCString& name, const moWCString& message)
{
	unsigned int	id;

	{
		moLockMutex lock(f_mutex);
		id = ++f_mark;
	}
	moTransactionSPtr mark = new BeginMark(id, name, message);
	AddTransaction(mark);
	return id;
}


void moTransactionManager::AddEndMark(mark_id_t id)
{
	zuint32_t level;
	moLockMutex lock(f_mutex);
	moList::position_t idx = f_undo.Count();
	while(idx > 0UL) {
		--idx;
		moTransaction *transaction = f_undo.Get(idx);
		BeginMark *begin_mark = dynamic_cast<BeginMark *>(transaction);
		if(begin_mark != 0) {
			if(begin_mark->GetID() == id) {
				// found the Begin! we can have a corresponding end
				if(level != 0UL) {
					// argh! there are missing ends
					// NOTE: should we auto-close missing ends?!
#ifdef _DEBUG
fprintf(stderr, "moTransactionManager: error: missing EndMark's while adding EndMark #%d\n", id);
#endif
					assert(0);
				}
				if(idx == f_undo.Count() - 1) {
					// an empty begin/end mark serves no purpose,
					// delete the begin!
					f_undo.Delete(idx);
				}
				else {
					moTransactionSPtr mark = new EndMark(begin_mark);
					AddTransaction(mark);
				}
				return;
			}
			if(level == 0UL) {
				// Found a BeginMark without any possibly corresponding EndMark
#ifdef _DEBUG
fprintf(stderr, "moTransactionManager: error: found BeginMark #%d without an EndMark while adding EndMark #%d\n", begin_mark->GetID(), id);
#endif
				assert(0);
			}
			else {
				--level;
			}
		}
		else {
			EndMark *end_mark = dynamic_cast<EndMark *>(transaction);
			if(end_mark != 0) {
				++level;
			}
		}
	}
}


void moTransactionManager::AddTransaction(moTransaction *transaction)
{
	if(transaction == 0) {
		return;
	}

	moLockMutex lock(f_mutex);

	SetFlag running(f_running);

	f_undo += *transaction;
	f_redo.Empty();
	transaction->DoIt();

	CallCallbacks(&moTransactionCallback::Transaction_Add);
}


void moTransactionManager::Undo(void)
{
	moTransactionSPtr	transaction;
	BeginMark		*begin_mark;
	EndMark			*end_mark;
	mark_id_t		id;

	moLockMutex lock(f_mutex);

	SetFlag running(f_running);

	// Undoing a BeginMark transaction has no effect, so we just delete
	// those (they are considered spurious)
	//
	// Undoing an EndMark transaction has the effect of undoing all
	// the transaction from that EndMark to the corresponding
	// BeginMark

	for(;;) {
		if(f_undo.IsEmpty()) {
			return;
		}
		transaction = f_undo.GetLast();
		begin_mark = dynamic_cast<BeginMark *>(static_cast<moTransaction *>(transaction));
		if(begin_mark == 0) {
			break;
		}
		// just ignore spurious begin marks
		f_undo.Delete(f_undo.Count() - 1);
	}
	end_mark = dynamic_cast<EndMark *>(static_cast<moTransaction *>(transaction));
	if(end_mark != 0) {
		id = end_mark->GetID();
		f_redo += *transaction;
		f_undo.Delete(f_undo.Count() - 1);
		for(;;) {
			if(f_undo.IsEmpty()) {
				break;
			}
			transaction = f_undo.GetLast();
			f_redo += *transaction;
			f_undo.Delete(f_undo.Count() - 1);
			begin_mark = dynamic_cast<BeginMark *>(static_cast<moTransaction *>(transaction));
			if(begin_mark != 0) {
				if(begin_mark->GetID() == id) {
					// we found the corresponding BeginMark
					break;
				}
				// just skip intermediate begin marks
			}
			else {
				end_mark = dynamic_cast<EndMark *>(static_cast<moTransaction *>(transaction));
				if(end_mark == 0) {
					// not an end mark, we can call Undo();
					transaction->Undo();
				}
			}
		}
	}
	else {
		f_redo += *transaction;
		f_undo.Delete(f_undo.Count() - 1);
		transaction->Undo();
	}

	CallCallbacks(&moTransactionCallback::Transaction_Undo);
}


void moTransactionManager::Redo(void)
{
	moTransactionSPtr	transaction;
	BeginMark		*begin_mark;
	EndMark			*end_mark;
	mark_id_t		id;

	moLockMutex lock(f_mutex);

	SetFlag running(f_running);

	// The Redo() is the inverse of the Undo() since things are
	// stacked in reverse order; thus EndMark's at the end of
	// the redo list are spurious (more so because you should
	// not be able to add a spurious end mark in any list)
	//
	// And if we find a BeingMark we execute all the transactions
	// up to the EndMark

	for(;;) {
		if(f_redo.IsEmpty()) {
			return;
		}
		transaction = f_redo.GetLast();
		end_mark = dynamic_cast<EndMark *>(static_cast<moTransaction *>(transaction));
		if(end_mark == 0) {
			break;
		}
		// just ignore spurious end marks
		// really this should never happen because the AddEndMark() avoids adding
		// spurious end marks...
		assert(0);
		f_redo.Delete(f_redo.Count() - 1);
	}

	f_undo += *transaction;
	f_redo.Delete(f_redo.Count() - 1);

	begin_mark = dynamic_cast<BeginMark *>(static_cast<moTransaction *>(transaction));
	if(begin_mark != 0) {
		id = begin_mark->GetID();
		for(;;) {
			if(f_redo.IsEmpty()) {
				break;
			}
			transaction = f_redo.GetLast();
			f_undo += *transaction;
			f_redo.Delete(f_redo.Count() - 1);
			end_mark = dynamic_cast<EndMark *>(static_cast<moTransaction *>(transaction));
			if(end_mark != 0) {
				if(end_mark->GetID() == id) {
					// we found the corresponding EndMark
					break;
				}
				// just skip intermediate end marks
			}
			else {
				begin_mark = dynamic_cast<BeginMark *>(static_cast<moTransaction *>(transaction));
				if(begin_mark == 0) {
					// not a begin mark, we can call DoIt();
					transaction->DoIt();
				}
			}
		}
	}
	else {
		transaction->DoIt();
	}

	CallCallbacks(&moTransactionCallback::Transaction_Redo);
}




/************************************************************ DOC:

CLASS

	moTransactionManager

NAME

	GetNextUndoTransaction -- return the most current undo transaction
	GetNextRedoTransaction -- return the most current redo transaction

SYNOPSIS

	moTransactionSPtr GetNextUndoTransaction(void) const;
	moTransactionSPtr GetNextRedoTransaction(void) const;

DESCRIPTION

	The transaction manager manages two lists. One for the undo
	and one for the redo transactions.

	It is possible to know of the next undo or redo transaction
	by calling these functions. This can be used to retrieve
	the name of the undo and/or redo to display in a menu or
	a status bar.

BUGS

	This function will eventually return a BeginMark which is
	an internal object. That internal object Undo() and DoIt()
	functions are not functional and cannot be called.

RETURN VALUES

	These functions return a transaction pointer or NULL whenever
	there is no transaction in the corresponding list.

SEE ALSO

	Undo(), Redo(), AddTransaction()

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
moTransactionSPtr moTransactionManager::GetNextUndoTransaction(void) const
{
	moList::position_t	idx;
	moTransactionSPtr	transaction;
	BeginMark		*begin_mark;
	EndMark			*end_mark;

	moLockMutex lock(f_mutex);

	// we must skip spurious begin mark transactions
	// if it is an end mark, return the corresponding begin mark
	idx = f_undo.Count();
	for(;;) {
		if(idx == 0UL) {
			// empty!
			return 0;
		}
		--idx;

		transaction = f_undo.Get(idx);
		end_mark = dynamic_cast<EndMark *>(static_cast<moTransaction *>(transaction));
		if(end_mark != 0) {
			return static_cast<BeginMark *>(end_mark->GetBeginMark());
		}
		begin_mark = dynamic_cast<BeginMark *>(static_cast<moTransaction *>(transaction));
		if(begin_mark == 0) {
			return transaction;
		}
	}
	/*NOTREACHED*/
}


moTransactionSPtr moTransactionManager::GetNextRedoTransaction(void) const
{
	moList::position_t	idx;
	moTransactionSPtr	transaction;
	EndMark			*end_mark;

	moLockMutex lock(f_mutex);

	idx = f_redo.Count();
	for(;;) {
		if(idx == 0UL) {
			// empty!
			return 0;
		}
		--idx;

		transaction = f_redo.Get(idx);
		end_mark = dynamic_cast<EndMark *>(static_cast<moTransaction *>(transaction));
		if(end_mark == 0) {
			// begin marks or other transaction returned as is
			return transaction;
		}
		// skipping spurious ends, thought they should never occur!
		// Spurious EndMark found in the f_redo list?!
		// this could happen if we get an exception "at the wrong time"
		assert(0);
	}
	/*NOTREACHED*/
}






/************************************************************ DOC:

CLASS

	moTransactionManager

NAME

	RegisterCallback -- add a call back on transaction events
	UnregisterCallback -- remove a previously registered callback
	MakeCurrent -- call the callbacks as required when the manager becomes current
	CallCallbacks -- call all the callbacks with the specified function

SYNOPSIS

	void RegisterCallback(moTransactionCallback *callback);
	void UnregisterCallback(moTransactionCallback *callback);
	void MakeCurrent(void);
	void CallCallbacks(moTransactionCallback::callback_function_t func);

PARAMETERS

	callback -- pointer to an object derived from moTransactionCallback
	func -- the offset of the callback to be called

DESCRIPTION

	The transaction manager sends events (call callbacks) so interested
	parties can track what is happening in the manager.

	The existing events are:

		Added a new transaction (Transaction_Add)
		Applied the Undo function (Transaction_Undo)
		Applied the Redo function (Transaction_Redo)

	The events are called AFTER the fact. You cannot currently
	prevent a transaction from happening (but since you are the
	one who decides to call the Undo() and Redo() functions of
	your transaction manager, you are still able to prevent
	such a call from happening!)

	The RegisterCallback() will register your callback in the
	manager. All registered callbacks get called each time one
	of the AddTransaction(), Undo() and Redo() functions is
	called. Warning: adding the same callback multiple times
	will in effect make the manager call your callback multiple
	times.

	The UnregisterCallback() remove a callback previously added
	using the RegisterCallback() function. You need to call this
	function the same number of time you called the register
	function to remove all instances.

	The MakeCurrent() function needs to be called whenever you
	change from one transaction manager to another. For instance,
	if you have an application with 3 different tabs, each tab
	can have its own transaction list (so as to avoid confusion
	and not undo things from a hidden tag.) Whenever the user
	switch to a new tab, you want to call MakeCurrent() on the
	transaction which becomes the current transaction in your
	application. The effect is to call CallCallbacks() with
	the Undo and the Redo callback lists.

	The CallCallbacks() function calls the specified function of
	all the currently registered callbacks. The transaction
	manager mutex will be locked while this function is running.

	For instance, the following calls the Transaction_Undo event:

	  CallCallbacks(&moTransactionCallback::Transaction_Undo);

	Note that the system currently uses a C array to store the
	callbacks. So a callback can remove itself or another callback
	safely even when called from the CallCallbacks() function.

NOTES

	The transactions call the callbacks while the manager is
	locked. Thus it is thread safe.

BUGS

	At this time, the system is limited to MAX_CALLBACKS in
	a transaction manager. It seems to me that only 1 is
	necessary and thus a small array of up to 5 is certainly
	more than plainty.

	It is considered a bug to try to register too many
	callbacks.

	The callbacks are used as bare pointers without a reference
	count. You must make sure that the order in which things
	are done prevent the calls from happening after your
	moTransactionCallback instances are destroyed.

	It is considered a bug to unregister a callback that
	you never registered.

SEE ALSO

	Undo(), Redo(), AddTransaction()

	moTransactionGroup class, moTransaction class,
	moBuildTransactionGroup class

*/
void moTransactionManager::RegisterCallback(moTransactionCallback *callback)
{
	int		idx;

	moLockMutex lock(f_mutex);

	for(idx = 0; idx < MAX_CALLBACKS; ++idx) {
		if(f_callbacks[idx] == 0) {
			f_callbacks[idx] = callback;
			return;
		}
	}

	// ERROR: too many callbacks for the transaction manager
	assert(0);
}


void moTransactionManager::UnregisterCallback(moTransactionCallback *callback)
{
	int		idx;

	moLockMutex lock(f_mutex);

	for(idx = 0; idx < MAX_CALLBACKS; ++idx) {
		if(f_callbacks[idx] == callback) {
			f_callbacks[idx] = 0;
			return;
		}
	}

	// ERROR: callback not currently registered with manager
	assert(0);
}


void moTransactionManager::MakeCurrent(void)
{
	moLockMutex lock(f_mutex);

	// its a new list, always call whatever the status
	// (i.e. a call is necessary even for an empty list)
	CallCallbacks(&moTransactionCallback::Transaction_Undo);
	CallCallbacks(&moTransactionCallback::Transaction_Redo);
}


void moTransactionManager::CallCallbacks(moTransactionCallback::callback_function_t func)
{
	int		idx;

	moLockMutex lock(f_mutex);

	for(idx = 0; idx < MAX_CALLBACKS; ++idx) {
		if(f_callbacks[idx] != 0) {
			(f_callbacks[idx]->*func)(this);
		}
	}
}










/************************************************************ DOC:

CLASS

	moTransactionManager::moTransactionCallback

NAME

	Destructor -- cleans up the transaction callback
	Transaction_Add -- called when a new transaction is added
	Transaction_Undo -- called when a new Undo transaction is added
	Transaction_Redo -- called when a new Redo transaction is added

SYNOPSIS

	virtual ~moTransactionCallback();
	virtual void Transaction_Add(moTransactionManager *transaction_manager);
	virtual void Transaction_Undo(moTransactionManager *transaction_manager);
	virtual void Transaction_Redo(moTransactionManager *transaction_manager);
	typedef void (moTransactionCallback::*callback_function_t)(moTransactionManager *transaction_manager);

DESCRIPTION

	This class is expected to be derived by the menus and status bar and other
	such items which need to know on the fly that the transaction lists are
	changing. This is usually useful to write what can be done and undone next
	or for the status bar what has just been done or undone.

	All the functions have a default "do nothing" implementation. You thus can
	derive just those that you need.

	The Transaction_Add() is the only one called whenever a transaction is
	added even though it is somewhat equivalent to the Transaction_Redo()
	(in the sense that the DoIt() function is called.)

	The Transaction_Undo() is called whenever the transaction manager
	Undo() is called.

	The Transaction_Redo() is called whenever the transaction manager
	Redo() is called.

SEE ALSO

	moTransactionManager::CallCallbacks()

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
moTransactionManager::moTransactionCallback::~moTransactionCallback()
{
}


void moTransactionManager::moTransactionCallback::Transaction_Add(moTransactionManager *transaction_manager)
{
}


void moTransactionManager::moTransactionCallback::Transaction_Undo(moTransactionManager *transaction_manager)
{
}


void moTransactionManager::moTransactionCallback::Transaction_Redo(moTransactionManager *transaction_manager)
{
}






/************************************************************ DOC:

CLASS

	moBuildTransactionGroup

NAME

	Contructor -- initializes a transaction group builder
	moGetClassName -- get the name of this class

SYNOPSIS

	moBuildTransactionGroup(moTransactionGroup *group);
	virtual const char *moGetClassName(void) const;

PARAMETERS

	group - a user allocated group

DESCRIPTION

	The moBuildTransactionGroup class is a helper class used
	to create a transaction group on the stack.

	The reality is that this object allocates a group
	with the new operator and manages it internally. You can
	also create your own group so as to give it a specific
	name and description.

	You MUST call Commit() to register the resulting group
	transaction. Otherwise the transaction will be lost.
	If you get an error while creating the transaction, but
	may still have access to the build transaction group object,
	you most certainly want to call the Rollback() function.
	This cancels the transaction and ensures you cannot commit
	it anymore.

	Steps to create a transaction group:

		Create an moBuildTransactionGroup (usually on the stack)
		Call AddTransaction() with each transaction
			you need in this group
		At any time, call Rollback() if something goes wrong
		At the end, Call Commit() if the transaction succeeded

	By default, the function will create an moTransactionGroup. You
	can also pass your own group pointer. In general, you want to
	do this when you need a dynamic name and message in a group:

		moBuildTransactionGroup grp(new MyTransactionGroup);
		grp.AddTransaction(...);
		...
		grp.Commit()

	The MyTransactionGroup is managed internally and thus gets deleted
	when not necessary anymore.

	The moGetClassName() function returns the name of the class.

NOTE

	If the Commit() function is not called before the moBuildTransactionGroup
	is destroyed, then it is assumed that the Rollback() function needed to
	be called instead. In other words: the transaction group will not
	automatically be commited.

	Once one of the Commit() or Rollback() functions has been called, you
	cannot add any more transactions.

BUGS

	The group passed to the contructor of an moBuildTransactionGroup
	object must be allocated with the new operator. In debug mode, the
	constructor will assert if this isn't the case.

RETURN VALUES

	The moGetClassName() function returns the name of the class:
	"molib::moBase::moBuildTransactionGroup".

SEE ALSO

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
moBuildTransactionGroup::moBuildTransactionGroup(moTransactionGroup *group)
{
	// User specified group?
	if(group) {
		assert(group->IsDynamicObject());
		f_group = group;
	}
	else {
		f_group = new moTransactionGroup;
	}
}


const char *moBuildTransactionGroup::moGetClassName(void) const
{
	return "molib::moBase::moBuildTransactionGroup";
}







/************************************************************ DOC:

CLASS

	moBuildTransactionGroup

NAME

	Empty -- clear the current group from any transaction
	IsEmpty -- test whether the group is empty
	AddTransaction -- add a transaction to the group

SYNOPSIS

	void Empty(void);
	bool IsEmpty(void) const;
	void AddTransaction(moTransaction *transaction);

PARAMETERS

	transaction -- a transaction to add to this group

DESCRIPTION

	The Empty() function removes all the entries which were
	inserted in the group so far.

	In a way, this is similar to a Rollback(), except that you
	can still add transactions afterward.

	Sample:

		Create an moBuildTransactionGroup
		Start adding transactions
		Detect a case which means previous transactions are no good
		Call Empty() to removes existing transactions
		Add new transactions
		Commit()

	The IsEmpty() function tests whether any transactions were
	already added in a group transaction.

	The AddTransaction() function adds a new transaction to the
	group transaction. Once all the necessary transactions have
	been added, call the Commit() function to add the group
	transaction into a transaction manager or the Rollback()
	function to cancel this group transaction.

BUGS

	You cannot call any of these functions after you commited
	or canceled the transaction.

RETURN VALUES

	The IsEmpty() function returns true if no transactions are
	present in the group (i.e. on initialization or after
	a call of the Empty() function.)

SEE ALSO

	Cancel(), Rollback()

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
void moBuildTransactionGroup::Empty(void)
{
	if(!f_group) {
		// ERROR: already commited or cancelled
		assert(0);
		return;
	}

	f_group->Empty();
}


bool moBuildTransactionGroup::IsEmpty(void) const
{
	if(!f_group) {
		// ERROR: already commited or cancelled
		assert(0);
		return true;
	}

	return f_group->IsEmpty();
}


void moBuildTransactionGroup::AddTransaction(moTransaction *transaction)
{
	if(!f_group) {
		// ERROR: already commited or cancelled
		assert(0);
		return;
	}

	f_group->AddTransaction(transaction);
}








/************************************************************ DOC:

CLASS

	moBuildTransactionGroup

NAME

	Commit -- add the group transaction to the manager
	Rollback -- cancel this group transaction

SYNOPSIS

	void Commit(moTransactionManager *transaction_manager);
	void Rollback(void);

PARAMETERS

	transaction_manager -- the transaction manager receiving this
		new group; this pointer cannot be set to NULL

DESCRIPTION

	The Commit() function calls the AddTransaction() function
	of the specified transaction manager with the group, unless
	the group transaction is empty.

	An empty group transaction does not need to be added to the
	transaction manager. For this reason it is simply dropped.

	Then the transaction group sets its f_group member to NULL
	meaning that it was commited.

	The Rollback() function deletes the group transaction.

	After either one of these calls, you cannot use the
	transaction group anymore.

BUGS

	You cannot call any more of the build transaction group
	functions once you commited or cancelled it. This will
	result in an assert() in debug mode or nothing will
	happen in release. On purpose, there is no function to
	recreate a new group to build a new transaction. Instead
	you need to create a new moBuildTransactionGroup object.

	If you want to cancel the current group and start over,
	use the Empty() function instead of Rollback().

SEE ALSO

	AddTransaction(), Empty(), IsEmpty()

	moTransactionGroup class, moTransactionManager class,
	moBuildTransactionGroup class

*/
void moBuildTransactionGroup::Commit(moTransactionManager *transaction_manager)
{
	assert(transaction_manager != 0);

	if(!f_group) {
		// ERROR: already commited or cancelled
		assert(0);
		return;
	}

	if(!f_group->IsEmpty()) {
		transaction_manager->AddTransaction(f_group);
	}

	f_group = 0;
}


void moBuildTransactionGroup::Rollback(void)
{
	// ERROR: already commited or cancelled
	assert(!!f_group);
	f_group = 0;
}




/************************************************************ DOC:

CLASS

	moTransactionBuilder

NAME

	Contructor -- initializes a transaction builder
	Destructor -- enables proper virtual destruction
	moGetClassName -- get the name of this class

SYNOPSIS

	moTransactionBuilder(void);
	virtual ~moTransactionBuilder();
	virtual const char *moGetClassName(void) const;

DESCRIPTION

	The TransactionBuilder class is a helper class used
	to create a transaction on the stack. You can add
	one or more transactions to it. If you added 2 or
	more, then this object will automatically create
	a group for you before to commit the transactions.

	You MUST call Commit() to register the resulting transaction.
	Otherwise the transaction will be lost. If you get an error
	while creating the transaction, but may still have access
	to the transaction builder object, you most certainly want
	to call the Rollback() function. This cancels the transaction
	and ensures you cannot commit it anymore.

	Steps to create a transaction builder:

		Create an moTransactionBuilder (usually on the stack)
		Call AddTransaction() with each transaction you need
		At any time, call Rollback() if something goes wrong
		At the end, Call Commit() if the transaction succeeded

	The moGetClassName() function returns the name of the class.

NOTE

	If the Commit() function is not called before the moTransactionBuilder
	is destroyed, then it is assumed that the Rollback() function needed to
	be called instead. In other words: the built transaction will not
	automatically be commited.

	Once one of the Commit() or Rollback() functions has been called, you
	cannot add any more transactions.

BUGS

	You cannot specify which type of group to create (we may add a
	virtual function later if this becomes necessary.) By default
	this object creates an moTransactionGroup.

RETURN VALUES

	The moGetClassName() function returns the name of the class:
	"molib::moBase::moTransactionBuilder".

SEE ALSO

	moTransactionGroup class, moTransactionManager class,
	moTransactionBuilder class

*/
moTransactionBuilder::moTransactionBuilder(void)
{
	//f_done -- auto-init
	//f_transactions -- auto-init
}


moTransactionBuilder::~moTransactionBuilder()
{
}


const char *moTransactionBuilder::moGetClassName(void) const
{
	return "molib::moBase::moTransactionBuilder";
}







/************************************************************ DOC:

CLASS

	moTransactionBuilder

NAME

	Empty -- clear the current list of transactions
	IsEmpty -- test whether the builder is empty
	AddTransaction -- add a transaction to the builder

SYNOPSIS

	void Empty(void);
	bool IsEmpty(void) const;
	void AddTransaction(moTransaction *transaction);

PARAMETERS

	transaction -- a transaction to add to this builder

DESCRIPTION

	The Empty() function removes all the entries which were
	inserted in the builder so far.

	In a way, this is similar to a Rollback(), except that you
	can still add transactions afterward.

	Sample:

		Create an moTransactionBuilder
		Start adding transactions
		Detect a case which means previous transactions are no good
		Call Empty() to remove previous transactions
		Add new transactions
		Commit()

	The IsEmpty() function tests whether any transactions were
	already added in a group transaction.

	The AddTransaction() function adds a new transaction to the
	transaction builder. Once all the necessary transactions have
	been added, call the Commit() function to add the transaction
	build into a transaction manager or the Rollback() function to
	cancel this transaction builder.

BUGS

	You cannot call any of these functions after you commited
	or canceled the transaction.

RETURN VALUES

	The IsEmpty() function returns true if no transactions are
	present in the builder (i.e. on initialization or after
	a call of the Empty() function.)

SEE ALSO

	Cancel(), Rollback()

	moTransactionGroup class, moTransactionManager class,
	moTransactionBuilder class

*/
void moTransactionBuilder::Empty(void)
{
	if(f_done) {
		// this transaction was commited or rolled back
		assert(0);
		return;
	}
	f_transactions.Empty();
}


bool moTransactionBuilder::IsEmpty(void) const
{
	if(f_done) {
		// this transaction was commited or rolled back
		assert(0);
		return true;
	}
	return f_transactions.IsEmpty();
}


void moTransactionBuilder::AddTransaction(moTransaction *transaction)
{
	if(f_done) {
		// this transaction was commited or rolled back
		assert(0);
		return;
	}

	f_transactions += *transaction;
}








/************************************************************ DOC:

CLASS

	moTransactionBuilder

NAME

	Commit -- add the built transaction to the manager
	Rollback -- cancel this transaction builder

SYNOPSIS

	void Commit(moTransactionManager *transaction_manager);
	void Rollback(void);

PARAMETERS

	transaction_manager -- the transaction manager receiving this
		new transaction; this pointer cannot be set to NULL

DESCRIPTION

	The Commit() function calls the AddTransaction() function
	of the specified transaction manager with the transaction,
	unless no transaction were added.

	An empty transaction does not need to be added to the
	transaction manager. For this reason it is simply dropped.

	The Rollback() function deletes the transaction.

	After either one of these calls, you cannot use the
	transaction builder anymore.

BUGS

	You cannot call any more of the transaction builder
	functions once you commited or cancelled it. This will
	result in an assert() in debug mode or nothing will
	happen in release. On purpose, there is no function to
	reset the transaction builder. Instead you need to
	create a new moTransactionBuilder object.

	If you want to cancel the current transaction and start
	over, use the Empty() function instead of Rollback().

SEE ALSO

	AddTransaction(), Empty(), IsEmpty()

	moTransactionGroup class, moTransactionManager class,
	moTransactionBuilder class

*/
void moTransactionBuilder::Commit(moTransactionManager *transaction_manager)
{
	assert(transaction_manager != 0);

	if(f_done) {
		// ERROR: already commited or cancelled
		assert(0);
		return;
	}

	unsigned long count = f_transactions.Count();
	if(count == 1) {
		// no need for a group
		transaction_manager->AddTransaction(f_transactions.GetFirst());
	}
	else if(count > 0) {
		// we need to create a group and copy the list in it
		moTransactionGroupSPtr group = new moTransactionGroup;
		group->MergeTransactions(f_transactions);
		transaction_manager->AddTransaction(group);
	}

	// we don't need this here anymore
	f_transactions.Empty();

	f_done = true;
}


void moTransactionBuilder::Rollback(void)
{
	// ERROR: already commited or cancelled
	assert(!f_done);
	f_transactions.Empty();
	f_done = true;
}







};			// namespace molib;

// vim: ts=8

