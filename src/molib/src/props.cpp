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



#ifdef MO_PRAGMA_INTERFACE
#pragma implementation "mo/mo_props.h"
#endif

#include	"mo/mo_props.h"

#include <typeinfo>

namespace molib
{


#ifdef _MSC_VER
#pragma warning(disable: 4355)
#endif



/*! \brief Initialize the object with the specified name.
 *
 * Because it is necessary to search for properties in a bag
 * of properties, one needs to create a property with a name
 * to search for it. This is done with an moPropFind.
 * 
 * This class is similar to the moProp class without any
 * pure virtual function and thus instantiable.
 *
 * \param name The new name for this property
 *
 * \sa moPropBag::Get(), moPropBag::Set(), moPropBag::Delete()
 *
 */
class MO_DLL_EXPORT moPropFind : public moProp
{
public:
				moPropFind(mo_name_t name)
					: moProp(name)
				{
				}
	virtual			~moPropFind()
				{
				}

	virtual moPropSPtr	Duplicate(void) const
				{
					return static_cast<moProp *>(0);
				}

	virtual prop_type_t	GetType(void) const
				{
					return MO_PROP_TYPE_UNKNOWN;
				}

	virtual const char *	GetObjectName(void) const
				{
					return "moBase::moProp::moPropFind";
				}
};



/*! \brief Callback whenever a property value changes.
 *
 * This function is the one present in the callback that one
 * can setup as a signal handler in a property.
 * 
 * The function will be called every time the value of the
 * property changes.
 *
 * \note 
 * Not available yet, there will certainly be ways to check
 * whether the function should be called. For instance, if
 * the value doesn't change (i.e. is set to the same value),
 * then the function will be called. Later we may have ways
 * to avoid these most certainly useless calls.
 *
 * \sa moProp::Signal(), moProp::AddHandler(), moProp::RemoveHandler(), moProp<name>::Set()
 *
 */
const char *moPropSignalHandler::moGetClassName() const
{
	return "molib::moBase::moPropSignalHandler";
}


void moPropSignalHandler::SignalValueChanged(moPropRef& prop, moBase *data)
{
	// do nothing by default
}


/*! \brief Initialize a property event
 *
 * The moPropEvent constructors will initialize the event
 * so it is ready for posting. The copy operator will
 * copy the specified event into a new event.
 * 
 * Note that a property is mandatory so the constructors
 * require one. However, you can later change that property
 * with the SetProperty() function.
 * 
 * To get the property assigned to an event use the GetProperty()
 * function.
 * 
 * Whenever you post an event, it is first going to be
 * duplicated and then added to the list of events of the
 * event pipe where the event is being posted. In order to
 * duplicate an event, the Duplicate() function is called.
 * This new an moEvent of the right type and returns its
 * pointer.
 *
 * \param prop The property this event references
 *
 * \bug 
 * The Duplicate() function returns a smart pointer referenced
 * only once. It is the caller responsability to ensure that
 * the reference count isn't lost too soon. This is usually
 * archieve by putting the result in a smart pointer:
 * 
 * \code
 * 	moEventPropSPtr evt;
 * 
 * 	evt = event.Duplicate();
 * \endcode
 * 
 * This works fine since the ownership of the resulting smart
 * pointer is transfered to the evt smart pointer object.
 *
 * \param[in] prop        The property that generates events.
 * \param[in] data        Some to send to the user at time of dispatch
 * \param[in] dispatcher  The dispatcher to be notified of an event
 *
 * \todo
 * We should cache the PROPERTY name.
 *
 * \sa
 * SetProperty(), GetProperty()
 *
 */
moPropEvent::moPropEvent(const moProp& prop, moBase *data, const moEventDispatcher *dispatcher)
	: moEvent("PROPERTY"),
	  f_prop(&prop),
	  f_data(data),
	  f_dispatcher(dispatcher)
{
}


/*! \brief Initialize a property event
 *
 * \param event An event to copy.
 *
 */
moPropEvent::moPropEvent(const moPropEvent& event)
	: moEvent(event),
	  f_prop(event.f_prop)
{
}


moPropEvent::~moPropEvent()
{
	// mainly here to tell people it's a virtual class
}

/*! \brief Duplication and event and return it
 *
 * \bug 
 * The Duplicate() function returns a smart pointer referenced
 * only once. It is the caller responsability to ensure that
 * the reference count isn't lost too soon. This is usually
 * archieve by putting the result in a smart pointer:
 * 
 * \code
 * 	moEventPropSPtr evt;
 * 
 * 	evt = event.Duplicate();
 * \endcode
 * 
 * This works fine since the ownership of the resulting smart
 * pointer is transfered to the evt smart pointer object.
 *
 * \return a pointer to a copy of the event, allocated on the heap
 *
 */
moEventSPtr moPropEvent::Duplicate(void) const
{
	return new moPropEvent(*this);
}


/************************************************************ DOC:

CLASS

	moPropEvent

NAME

	SetProperty - change the property pointer of an moPropEvent
	GetProperty - get the property pointer of an moPropEvent

SYNOPSIS

	moPropSPtr GetProperty(void) const;
	void SetProperty(const moProp& prop);

PARAMETERS

	prop - a property to set in this event

DESCRIPTION

	On construction, an moPropEvent will be assigned a property.
	The same event can, at a later time, be assigned a different
	property with the SetProperty() function.

	To know what the current property of an moPropEvent is, use
	the GetProperty() function. Note that this function returns a
	smart pointer and the result should be saved in a smart
	pointer for security reasons.

NOTES

	If you want to save a list of moPropEvent's then you can
	use the moListOfPropEvent type. Note that list of events
	aren't sorted (an event pipe is usually behaving like a
	FIFO).

RETURN VALUE

	The GetProperty() function returns a smart pointer to a property.

SEE ALSO

	Constructor, Destructor

*/
void moPropEvent::SetProperty(const moProp& prop)
{
	f_prop = &prop;
}


moPropSPtr moPropEvent::GetProperty(void) const
{
	return f_prop;
}





/************************************************************ DOC:

CLASS

	moPropEvent

NAME

	SetData - change the data pointer of an moPropEvent
	GetData - get the data pointer of an moPropEvent

SYNOPSIS

	moDataSPtr GetData(void) const;
	void SetData(const moBase *data);

PARAMETERS

	data - any moBase object

DESCRIPTION

	On construction, an moPropEvent can be assigned a data pointer
	(it can also be null). It is possible to change that data
	pointer at a later time with SetData() and to retrieve it
	with GetData().

RETURN VALUE

	The GetData() function returns a smart pointer to an
	arbitrary object (i.e. moBase *)

SEE ALSO

	Constructor, Destructor

*/
void moPropEvent::SetData(const moBase *data)
{
	f_data = data;
}


moBaseSPtr moPropEvent::GetData(void) const
{
	return f_data;
}





/************************************************************ DOC:

CLASS

	moPropEvent

NAME

	SetDispatcher - change the dispatcher for this event
	GetDispatcher - retrieve the current dispatcher for this event

SYNOPSIS

	void SetDispatcher(const moEventDispatcher *dispatcher);
	moEventDispatcherSPtr GetDispatcher(void) const;

PARAMETERS

	dispatcher - the new dispatcher or null

DESCRIPTION

	On construction, an moPropEvent can be assigned a dispatcher.
	The same event can, at a later time, be assigned a different
	dispatcher with the SetDispatcher() function.

	To know what the current dispatcher of an moPropEvent is, use
	the GetDispatcher() function. Note that this function returns
	a smart pointer and the result should be saved in a smart
	pointer for security reasons.

RETURN VALUE

	The GetDispatcher() function returns a smart pointer to
	an moEventDispatcher object

SEE ALSO

	Constructor, Destructor

*/
void moPropEvent::SetDispatcher(const moEventDispatcher *dispatcher)
{
	f_dispatcher = dispatcher;
}


moEventDispatcherSPtr moPropEvent::GetDispatcher(void) const
{
	return f_dispatcher;
}









/************************************************************ DOC:

CLASS

	moProp

NAME

	Constructors - initialize or copy a property
	Destructor - cleans up the property
	operator = - copy a property value

SYNOPSIS

	moProp(mo_name_t name);
	moProp(const moProp& prop);
	virtual ~moProp();
	void moProp::Copy(const moProp& prop);

PARAMETERS

	name - the name of the property as a string or a number
	prop - a property to copy in 'this'

DESCRIPTION

	The moProp object is a low level property definition. This
	object can't hold a value. To do so, you need to derive it.

	One of the moProp constructors expect a name. This is name
	needs to be defined in the moNamePool.

	The other moProp constructor expects another property. In
	this case, the source property is copied in 'this' property.
	The copy includes the name, lock type flag, and list of
	handlers.

	The moProp destructor is declared here to show that it is
	part of the virtual function. It will clear the resources
	used by the property which in the low level is all automatic
	at the time of writing.

	The Copy() function is very dissimalar to the copy
	constructor. The assignment will NOT copy anything except
	the value.

BUGS

	The constructors and destructor are NOT public. There are
	defined as such right now, but this will most certainly
	change in the future. You can't create a property, but
	by using its corresponding reference.

RETURN VALUE

	The assignment operator returns a reference to 'this'.

SEE ALSO

	GetName, GetType, CopyFrom

*/
const char *moProp::moGetClassName(void) const
{
	return "molib::moBase::moProp";
}


moProp::moProp(mo_name_t name)
	: f_name(name),
	  f_lock_type(true)
{
//fprintf(stderr, "Created prop 0x%08X\n", (int)name);
}


moProp::moProp(const moProp& prop)
	: moBase(*this),
	  f_name(prop.GetName()),
	  f_lock_type(prop.IsTypeLocked())
	  // It seems that we never want another property handlers!
	  //f_handlers(prop.f_handlers),
	  //f_new_handlers(prop.f_new_handlers)
{
}


moProp::~moProp()
{
}



// note: when this happens, all we need to copy is the value
void moProp::Copy(const moProp& prop)
{
	// f_mutex -- can't be copied, it's specific to each property
	// f_name -- that's const anyway
	// f_lock_type -- should we copy this?! Not too sure
	// f_handlers -- we don't want to copy those
	// f_new_handlers -- we don't want to copy those
}





/************************************************************ DOC:

CLASS

	moProp

NAME

	Lock - lock this property
	Unlock - unlock this property

SYNOPSIS

	void Lock(void) const;
	void Unlock(void) const;

DESCRIPTION

	Lock or unlock a property so you are the only thread using
	it. This is useful, for instance, to apply an assignment
	operator such as +=. In this case you need to lock, Get()
	the value, add the parameter, Set() the result and then
	you can unlock the property (this way, the assignment operator
	become an atomic function).

SEE ALSO

	class moPropAutoLock

*/
void moProp::Lock(void) const
{
	f_mutex.Lock();
}

void moProp::Unlock(void) const
{
	f_mutex.Unlock();
}



/************************************************************ DOC:

CLASS

	moProp

NAME

	GetName - get the name of the property
	GetType - get the type of the property
	Compare - compare the properties by name

SYNOPSIS

	mo_name_t GetName(void) const;
	virtual prop_type_t GetType(void) const;
	virtual compare_t Compare(const moBase& object) const;

PARAMETERS

	object - another property of any type

DESCRIPTION

	Each property must be assigned a name. That name can later
	be queried (usually when enumerating a property bag). The
	name is a unique number which can be converted to a string
	using the moNamePool object:

	  moString name = moNamePool::GetNamePool()[prop->GetName()];

	The type of a property is defined as one of the following:

		MO_PROP_TYPE_UNKNOWN
		MO_PROP_TYPE_PROP_BAG
		MO_PROP_TYPE_INT
		MO_PROP_TYPE_LONG_LONG
		MO_PROP_TYPE_FLOAT
		MO_PROP_TYPE_DOUBLE
		MO_PROP_TYPE_STRING
		MO_PROP_TYPE_BINARY
		MO_PROP_TYPE_ARRAY

	Note that the type UNKNOWN should never be returned.

	The type ARRAY is an agregate of other types. That is, each
	element of an array can be of any type (including an array).
	Note that the array type supports the [] operator and thus
	one can use that operator to access its content.

	The Compare() function is used to be able to sort the properties
	in a list using their name. Properties of different types can
	be sorted among each others since only their name is used to
	do so.

NOTE

	Even though the Compare() function is virtual, it can't be
	overloaded in any specialized property. This definition has
	to be viewed as the final declaration.

RETURN VALUE

	GetName() returns a name number as defined in the moNamePool class
	GetType() returns an MO_PROP_TYPE_... value
	Compare() returns one of the MO_BASE_COMPARE_... value

SEE ALSO

	?

*/
moBase::compare_t moProp::Compare(const moBase& object) const
{
	const moProp& prop = dynamic_cast<const moProp&>(object);

//fprintf(stderr, "THIS.F_NAME = %d\n", f_name.is_initialized());
//fprintf(stderr, "OBJECT.F_NAME = %d\n", prop.f_name.is_initialized());

// the following should never happen
	assert(f_name != 0 && prop.f_name != 0);
	if(f_name == 0 || prop.f_name == 0) {
		return MO_BASE_COMPARE_UNORDERED;
	}

// compare the name which is a name
	return CompareInt(f_name, prop.f_name);
}





/************************************************************ DOC:

CLASS

	moProp::moHandler

NAME

	Constructor - initialize a handler holder
	Call - call the handler
	ToBeAdded - whether we need to add or remove this handler
	Compare - compare two handlers

SYNOPSIS

	moHandler(const moBase& handler, const moBase *data);

	void Call(moProp *prop);

	virtual compare_t Compare(const moBase& object) const;

PARAMETERS

	handler - an event or a signal handler
	data - this usually is a pointer to an moEventDispatcher
		whenever the handler is an event
	prop - the property on which this call occurs

DESCRIPTION

	The properties can have handlers which are called whenever an
	event occur. At this time, the only event is "this property
	value has been modified".

	Whenever the handlers have to be called, the Call() function
	will be used.

	In order to protect the handler lists (so a handler can add
	and/or remove handlers), we use two lists in each property.
	One list is the current handler list and the other is the
	add/remove list of handlers.

	Whenever the Signal() function of the moProp is called, it
	takes the add/remove list and applies that to the current
	list. Then the Call() function is called for each handler
	of the current list.

	Handlers are not sorted (i.e. the last appended will be the
	last called). They are searched by address in the moList.

SEE ALSO

	Class moProp

*/
moProp::moHandler::moHandler(const moBase& handler, const moBase *data)
	: f_handler(const_cast<moBase *>(&handler)),
	  f_data(data)
{
}

void moProp::moHandler::Call(moProp *prop)
{
	moPropSignalHandler *signal_handler;
	signal_handler = dynamic_cast<moPropSignalHandler *>(static_cast<moBase *>(f_handler));
	if(signal_handler != 0) {
		switch(prop->GetType()) {
		case MO_PROP_TYPE_UNKNOWN:
		case MO_PROP_TYPE_max:
			// we shouldn't be able to signal these types of properties
			assert(0);
			break;

		case MO_PROP_TYPE_PROP_BAG:
		{
			moPropBagRef p(*dynamic_cast<moPropBag *>(prop));
			signal_handler->SignalValueChanged(p, f_data);
		}
			break;

		case MO_PROP_TYPE_INT:
		{
			moPropIntRef p(*dynamic_cast<moPropInt *>(prop));
			signal_handler->SignalValueChanged(p, f_data);
		}
			break;

		case MO_PROP_TYPE_LONG_LONG:
		{
			moPropLongLongRef p(*dynamic_cast<moPropLongLong *>(prop));
			signal_handler->SignalValueChanged(p, f_data);
		}
			break;

		case MO_PROP_TYPE_FLOAT:
		{
			moPropFloatRef p(*dynamic_cast<moPropFloat *>(prop));
			signal_handler->SignalValueChanged(p, f_data);
		}
			break;

		case MO_PROP_TYPE_DOUBLE:
		{
			moPropDoubleRef p(*dynamic_cast<moPropDouble *>(prop));
			signal_handler->SignalValueChanged(p, f_data);
		}
			break;

		case MO_PROP_TYPE_POINTER:
		{
			moPropPointerRef p(*dynamic_cast<moPropPointer *>(prop));
			signal_handler->SignalValueChanged(p, f_data);
		}
			break;

		case MO_PROP_TYPE_STRING:
		{
			moPropStringRef p(*dynamic_cast<moPropString *>(prop));
			signal_handler->SignalValueChanged(p, f_data);
		}
			break;

		case MO_PROP_TYPE_BINARY:
		{
			moPropBinaryRef p(*dynamic_cast<moPropBinary *>(prop));
			signal_handler->SignalValueChanged(p, f_data);
		}
			break;

		case MO_PROP_TYPE_ARRAY:
		{
			moPropArrayRef p(*dynamic_cast<moPropArray *>(prop));
			signal_handler->SignalValueChanged(p, f_data);
		}
			break;

		}
	}

	moEventPipe *event_handler;
	event_handler = dynamic_cast<moEventPipe *>(static_cast<moBase *>(f_handler));
	if(event_handler != 0) {
		moPropEvent event(*prop, f_data, dynamic_cast<moEventDispatcher *>(static_cast<moBase *>(f_data)));
		event_handler->Post(event);
	}
}


moBase::compare_t moProp::moHandler::Compare(const moBase& object) const
{
	return f_handler->ComparePtr(dynamic_cast<const moHandler&>(object).f_handler);
}





/************************************************************ DOC:

CLASS

	moProp

NAME

	AddHandler - add a user handler
	RemoveHandler - remove a previously added user handler
	Signal - send a signal to all the event and signal handlers

SYNOPSIS

	void AddHandler(moBase& handler, moBase *data = 0);
	void RemoveHandler(moBase& handler);
	void Signal(void);

PARAMETERS

	handler - an moEventPipe or moPropSignalHandler
	data - any mo object to attach to this handler or null

DESCRIPTION

	These functions will allow a user to get signals whenever
	a property is being modified (written to). [we may later
	add additional signals such as "deleted", "created", "read",
	"resized", etc.]

	Signal Handlers: (moPropSignalHandler)

	Signal handlers are simple C++ function calls. This means
	the signal function will run in the current thread which
	can be a problem. However, it is much faster than the
	event pipes.

	Event Pipes: (moEventPipe, see mo_thread.h)

	Event pipes expect to receive messages which are then read
	by a thread. This enables you to have handlers being called
	in a safe manner in a multi-threaded environment.

	The Signal() function is protected from modifications to
	the list of handlers while running the current handlers.
	This means you can safely call the AddHandler() and
	RemoveHandler() functions within a handler. Note that new
	handlers won't be taken in account until the next time
	Signal() is called.

	The AddHandler() will add that handler to this property.
	The handler will be used each time the property is modified.
	When the AddHandler() is used to add an moEventPipe, one can
	set an object of type moEventDispatcher as the data parameter.
	This will give the event a dispatcher which is can execute
	by calling the moEvent::Dispatch() function.

	The RemoveHandler() will remove a handler which was previously
	added with AddHandler(). Note that the reference needs to be
	the same. Also, for each call to AddHandler() you will need to
	call RemoveHandler() once.

	The Signal() function is usually only called internally but
	can also be called by the end user at any time to trigger the
	signals.

NOTE

	For each handler which is added to a property, there is
	a call to AddRef().

	For each handler which is removed from a property, there
	is a call to Release().

	This means you need to remove all the handlers that you
	have added before.

SEE ALSO

	GetName, GetType

*/
void moProp::AddHandler(moBase& handler, const moBase *data)
{
	if(!handler.IsDynamicObject()) {
		throw moError("moProp::AddHandler(): Only dynamic objects are acceptable as property handlers -- object of type \"%s\"/\"%s\" refused (i.e. can't be objects from the stack, use the new operator instead)", handler.moGetClassName(), typeid(handler).name());
	}

	moHandlerSPtr holder = new moHandler(handler, data);

//fprintf(stderr, "PROP HANDLER AddHandler()\n");
	moLockMutex lock(f_mutex);
	f_handlers += holder;
	f_signal_list_changed = true;
	AddRef();
}


void moProp::RemoveHandler(moBase& handler)
{
	moList::position_t	found;

	if(!handler.IsDynamicObject()) {
		// Note: I would hope this never ever happens since
		// the AddHandler() already failed and thus there should
		// never be a RemoveHandler() call!
		throw moError("moProp::RemoveHandler(): Only dynamic object are acceptable as property handlers (i.e. can't be stack objects, use the new operator instead)");
	}

	moHandlerSPtr holder = new moHandler(handler, 0);

//fprintf(stderr, "PROP HANDLER RemoveHandler()\n");
	moLockMutex lock(f_mutex);
	found = f_handlers.Find(holder);
	if(found != moList::NO_POSITION) {
#if 0
{
moHandlerSPtr h = f_handlers.Get(found);
printf("Holder RC = %ld\n", h->ReferenceCount());
}
#endif
		f_handlers.Delete(found);
		f_signal_list_changed = true;
		Release();
	}
}



void moProp::Signal(void)
{
	moList::position_t	pos, max;

#if 0
fprintf(stderr, "PROP HANDLER Signal()\n");
	{ // tweak the lists while locked
		moLockMutex lock(f_mutex);

		max = f_new_handlers.Count();
		for(pos = 0; pos < max; pos++) {
			moHandlerSPtr holder = f_new_handlers.Get(pos);
			if(holder->ToBeAdded()) {
				f_handlers += holder;
			}
			else {
				found = f_handlers.Find(holder);
				if(found != moList::NO_POSITION) {
{
moHandlerSPtr h = f_handlers.Get(found);
fprintf(stderr, "Holder RC = %ld\n", h->ReferenceCount());
}
					f_handlers.Delete(found);
				}
			}
		}
		f_new_handlers.Empty();
	} // UNLOCKING
#endif

	f_mutex.Lock();
	do {
		f_signal_list_changed = false;
		max = f_handlers.Count();
		for(pos = 0; pos < max && !f_signal_list_changed; pos++) {
			moHandlerSPtr holder = f_handlers.Get(pos);
			f_mutex.Unlock();
			holder->Call(this);
			f_mutex.Lock();
		}
	} while(f_signal_list_changed);
	f_mutex.Unlock();
}



/************************************************************ DOC:

CLASS

	moProp

NAME

	LockType - prevent type from being modified
	UnlockType - allow type from being modified
	IsTypeLocked - check whether the type is locked

SYNOPSIS

	void LockType(void);
	void UnlockType(void);
	bool IsTypeLocked(void) const;

DESCRIPTION

	The LockType() and UnlockType() decide whether the type
	of a property can be changed with a call to the
	moPropBag::Set() function. The default is to have all
	properties have their type locked since it usually is
	not wanted to be changed.

	One can use the IsTypeLocked() function to know whether
	the type is currently lock.

RETURN VALUE

	The IsTypeLocked() function returns true if it is permitted
	to change the type of this property.

BUGS

	When one changes the type of a property, one removes
	the callbacks present on the old property. By default,
	however, one can't change the type of a property.

SEE ALSO

	moPropBag::Set

*/
void moProp::LockType(void)
{
	moLockMutex lock(f_mutex);

	f_lock_type = true;
}


void moProp::UnlockType(void)
{
	moLockMutex lock(f_mutex);

	f_lock_type = false;
}


/************************************************************ DOC:

CLASS

	moPropBag

NAME

	Constructor - create a property bag
	Destructor - no specific clean up is required for this class

SYNOPSIS

	protected:
	moPropBag(mo_name_t name);
	moPropBag(const moPropBag& bag, bool recursive = false);
	//virtual ~moPropBag();
	virtual void Copy(const moPropBag& bag);
	virtual moProp *Duplicate(void) const;

PARAMETERS

	name - the name of this property
	bag - the property bag to duplicate
	recursive - a flag to tell whether the copy should be recursive

DESCRIPTION

	The constructors initializes a new property bag from a name
	or another bag. The copy operator will copy all of the
	properties held by the source bag, recursively if the
	recursive flag is set to true.

	The assignment operator copies the value of the source bag
	into 'this' bag.

SEE ALSO

	Get, Set, Delete

*/
const char *moPropBag::moGetClassName(void) const
{
	return "molib::moProp::moPropBag";
}


moPropBag::moPropBag(mo_name_t name)
	: moProp(name)
{
}


moPropBag::moPropBag(const moPropBag& bag, bool recursive)
	: moProp(bag)
{
	moList::position_t	idx, max;

	max = bag.Count();
	if(recursive) {
		for(idx = 0; idx < max; ++idx) {
			f_props += *bag.f_props[idx].Duplicate();
		}
	}
	else {
		for(idx = 0; idx < max; ++idx) {
			f_props += bag.f_props[idx];
		}
	}
}


void moPropBag::Copy(const moProp& prop)
{
	moList::position_t	idx, max;

	if(this == &prop) {
		return;
	}

	// lower level copy first
	moProp::Copy(prop);

	// in this case it's a total overwrite!
	// (but for now it's not a recursive copy...)
	f_props.Empty();

	const moPropBag& bag = dynamic_cast<const moPropBag&>(prop);
	max = bag.Count();
	for(idx = 0; idx < max; ++idx) {
		f_props += bag.f_props[idx];
	}

	return;
}


moPropSPtr moPropBag::Duplicate(void) const
{
	moPropSPtr bag = new moPropBag(GetName());
	bag->Copy(*this);
	return bag;
}





/************************************************************ DOC:

CLASS

	moPropBag

NAME

	Dump - print a complete bag to stderr

SYNOPSIS

	void Dump(unsigned int flags = DUMP_FLAG_RECURSIVE, const char *message = 0) const;

	private:
	void DumpProps(unsigned int flags, unsigned int indent);
	void DumpProp(moPropSPtr prop);

PARAMETERS

	flags - a set of DUMP_FLAG_...
	message - a message to print at the start and end

DESCRIPTION

	This function will dump the property bag in stdout in a text
	format.

SEE ALSO

	?

*/
void moPropBag::Dump(unsigned int flags, const char *message) const
{
	if(message == 0) {
		message = "****************************** DUMP ******************************";
	}
	fprintf(stderr, "START: %s\n", message);
	DumpProps(flags, 0);
	fprintf(stderr, "--END: %s\n", message);
}

void moPropBag::DumpProps(unsigned int flags, unsigned int indent) const
{
	moList::position_t	idx, max;

	max = f_props.Count();
	for(idx = 0; idx < max; ++idx) {
		DumpProp(flags, indent, f_props.Get(idx));
	}
}

void moPropBag::DumpProp(unsigned int flags, unsigned int indent, moPropSPtr prop) const
{
	moList::position_t	idx, max;
	moPropSPtr		item;
	const char		*type;
	const char		*s;
	int			item_no;
	moBaseSPtr		p;

	switch(prop->GetType()) {
	case MO_PROP_TYPE_UNKNOWN:
		type = "Unknown";
		break;

	case MO_PROP_TYPE_PROP_BAG:
		type = "Bag";
		break;

	case MO_PROP_TYPE_INT:
		type = "Int";
		break;

	case MO_PROP_TYPE_LONG_LONG:
		type = "LongLong";
		break;

	case MO_PROP_TYPE_FLOAT:
		type = "Float";
		break;

	case MO_PROP_TYPE_DOUBLE:
		type = "Double";
		break;

	case MO_PROP_TYPE_POINTER:
		type = "Pointer";
		break;

	case MO_PROP_TYPE_STRING:
		type = "String";
		break;

	case MO_PROP_TYPE_BINARY:
		type = "Binary";
		break;

	case MO_PROP_TYPE_ARRAY:
		type = "Array of Properties";
		break;

	default:
	case MO_PROP_TYPE_max:
		// ERROR: this type should never be used!?
		assert(0);
		/*NOTREACHED*/
		break;

	}

	moWCString info;
	info = moWCString::Format("Name: \"%s\" (%d), type: \"%S\" (%d)",
		static_cast<const moWCString&>(moName(prop->GetName())).Data(),
		prop->GetName() & 0x3FFFFFFF, type, prop->GetType());
	s = info.SavedMBData();
	fprintf(stderr, "%*s%s", indent, "", s);
	switch(prop->GetType()) {
	case MO_PROP_TYPE_UNKNOWN:
	case MO_PROP_TYPE_max:
		fprintf(stderr, ", no value\n");
		break;

	case MO_PROP_TYPE_PROP_BAG:
		if((flags & DUMP_FLAG_RECURSIVE) != 0) {
			fprintf(stderr, "\n");
			dynamic_cast<moPropBag *>(static_cast<moProp *>(prop))->DumpProps(flags, indent + 2);
		}
		else {
			fprintf(stderr, ", value: not recursive\n");
		}
		break;

	case MO_PROP_TYPE_INT:
		fprintf(stderr, ", value: %d\n", static_cast<int>(dynamic_cast<moPropInt *>(static_cast<moProp *>(prop))->Get()));
		break;

	case MO_PROP_TYPE_LONG_LONG:
		fprintf(stderr, ", value: %lld\n", static_cast<long long>(dynamic_cast<moPropLongLong *>(static_cast<moProp *>(prop))->Get()));
		break;

	case MO_PROP_TYPE_FLOAT:
		fprintf(stderr, ", value: %g\n", static_cast<float>(dynamic_cast<moPropFloat *>(static_cast<moProp *>(prop))->Get()));
		break;

	case MO_PROP_TYPE_DOUBLE:
		fprintf(stderr, ", value: %g\n", static_cast<double>(dynamic_cast<moPropDouble *>(static_cast<moProp *>(prop))->Get()));
		break;

	case MO_PROP_TYPE_POINTER:
		p = dynamic_cast<moPropPointer *>(static_cast<moProp *>(prop))->Get();
		fprintf(stderr, ", value: %p (Class: %s)\n", p.operator moBase * (), p->moGetClassName());
		break;

	case MO_PROP_TYPE_STRING:
		s = dynamic_cast<moPropString *>(static_cast<moProp *>(prop))->Get().SavedMBData();
		fprintf(stderr, ", value: \"%s\"\n", s);
		break;

	case MO_PROP_TYPE_BINARY:
		fprintf(stderr, ", value: ... (TODO)\n");
		break;

	case MO_PROP_TYPE_ARRAY:
	{
		moPropArray *array = dynamic_cast<moPropArray *>(static_cast<moProp *>(prop));
		max = array->CountIndexes();
		fprintf(stderr, ", value: %d items\n", (int) max);
		for(idx = 0; idx < max; ++idx) {
			item_no = array->ItemNoAtIndex(idx);
			printf("    %*sElement: %d (Item: %d)\n", indent, "", (int) idx, item_no);
			item = array->GetAtIndex(idx);
			DumpProp(flags, indent + 8, item);
		}
	}
		break;

	}
}


/************************************************************ DOC:

CLASS

	moPropBag

NAME

	Get - retrieve a property of a bag
	Set - set a property in a bag
	Delete - delete a property from a bag

SYNOPSIS

	moPropSPtr Get(int index_or_name) const;
	bool Set(int index_or_name, const moProp& prop, bool overwrite = true);
	void Delete(int index_or_name);

PARAMETERS

	index_or_name - the property by index or name
	prop - the property to save in a bag
	overwrite - whether an existing property gets overwritten

DESCRIPTION

	The Get() functions will be used to read the content of
	a property bag.

	Using an integer, you can either enumerate the content of a
	bag using values from 0 to Count() - 1, or use the name of
	a property, name number obtained using the moNamePool.

	The Set() function copies your property in an existing or a
	new property in the bag.

	. New Property:

	A new property is created when a property of that name doesn't
	exist yet in the bag or when the type of the property you are
	setting is not the same as the existing property type. Note
	that by default the type of a property can't be changed. You
	need to UnlockType() of the property before you can do so.

	. Existing Property:

	First of all, if there is a property with the same name and
	the overwrite flag is set to false, then nothing happens.

	When there is an existing property of the same type, just the
	value is being set. Otherwise, the existing property will be
	deleted (And everything, including its callbacks, will be
	lost) before the new one is copied in its place. This is why
	by default it is not possible to change the type of a property.

	. All Cases:

	In any even, the callbacks are NEVER copied from one property
	to another. This may change in the future, but the only reason
	for such a need would be if we want to dynamically change the
	type of a property, but it doesn't seem very useful.

	The Delete() function will search for the specified index or
	name and delete that property from the property bag. If the
	property doesn't exist yet, nothing happens.

RETURN VALUES

	The Get() function returns a smart pointer to a property
	reference which can be NULL if the property doesn't currently
	exist in the property bag.

	The Set() function returns 'true' whenever the value of the
	property changed. If the value is not modified (because it
	can't be overwritten, the type would need to be changed, etc.)
	then the function returns 'false'.

BUGS

	When one changes the type of a property, one removes
	the callbacks present on the old property.

	The Get() and Set() functions will throw an error when
	index_or_name is an error or out of bounds. The Delete()
	function won't on an out of bounds error, it still will
	on an error name.

SEE ALSO

	Count, Empty

*/
moPropSPtr moPropBag::Get(int index_or_name) const
{
	moLockMutex lock(f_mutex);

	if(moNamePool::IsUser(index_or_name)) {
		return &f_props[index_or_name];
	}

	if(moNamePool::IsName(index_or_name)) {
		moPropFind n(index_or_name);
		moList::position_t pos = f_props.Find(&n);
		if(pos != moList::NO_POSITION) {
			return &f_props[pos];
		}
		// property not found
		return static_cast<moProp *>(0);
	}

	throw moError("moPropBag::Get(): invalid index or name, this looks more like an error number");
}


bool moPropBag::Set(int index_or_name, const moProp& prop, bool overwrite)
{
	if(&prop == 0) {
		throw moError("moPropBag::Set(): trying to add a null pointer property");
	}

	moPropSPtr	p;

	moLockMutex	lock(f_mutex);

	if(moNamePool::IsUser(index_or_name)) {
		if(static_cast<unsigned long>(index_or_name) >= Count()) {
			throw moError("moPropBag::Set(): position out of bounds");
		}
		if(!overwrite) {
			return false;
		}
		p = f_props.Get(index_or_name);
	}
	else if(moNamePool::IsName(index_or_name)) {
		moPropFind n(index_or_name);
		index_or_name = f_props.Find(&n);
		if(static_cast<moList::position_t>(index_or_name) != moList::NO_POSITION) {
			if(!overwrite) {
				return false;
			}
			p = f_props.Get(index_or_name);
		}
	}
	else {
		throw moError("moPropBag: invalid index or name, this looks more like an error number");
	}

	// create a new property whenever the type of the
	// property changes or when there isn't a property
	// of that type available
	if(p == 0) {
		// create new prop
		f_props += *static_cast<moProp *>(prop.Duplicate());
	}
	else if(p->GetType() != prop.GetType()) {
		if(p->IsTypeLocked()) {
			return false;
		}
		// create a new prop with proper type
		if(static_cast<moList::position_t>(index_or_name) != moList::NO_POSITION) {
			f_props.Delete(index_or_name);
		}
		f_props += * static_cast<moProp *>(prop.Duplicate());
	}
	else {
		// just overwrite value of existing property
		p->Copy(prop);
	}

	return true;
}



void moPropBag::Delete(int index_or_name)
{
	moLockMutex	lock(f_mutex);

	if(moNamePool::IsUser(index_or_name)) {
		if(static_cast<unsigned long>(index_or_name) >= Count()) {
			return;
		}
	}
	else if(moNamePool::IsName(index_or_name)) {
		moPropFind n(index_or_name);
		index_or_name = f_props.Find(&n);
		if(static_cast<moList::position_t>(index_or_name) == moList::NO_POSITION) {
			return;
		}
	}
	else {
		throw moError("moPropBag::Delete(): invalid index or name, this looks more like an error number");
	}

	f_props.Delete(index_or_name);

	return;
}





/************************************************************ DOC:

CLASS

	private:
	moPropArray::moItem

NAME

	moItem -- constructor which initializes an moItem
	Compare -- compare two items by item number
	operator = -- copy an item in another with the same number
	GetItemNo -- get the item number
	Get -- get the item property

SYNOPSIS

	private:
	moItem(int item_no, const moProp *prop);
	virtual compare_t Compare(const moBase& object) const;
	moItem& operator = (const moItem& item);
	int GetItemNo(void) const;
	moPropSPtr Get(void) const;

PARAMETERS

	item_no - this item number
	prop - this item property
	object - another item
	item - another item

DESCRIPTION

	An array defines a list of properties internally. This list
	has items named moItem. These items are sorted using their
	item number (item_no) and they hold a pointer to a property.

	The type of the property held by an array item can be
	anything.

RETURN VALUES

	The Compare() returns one of the moBase compare values
	comparing the item number of the two specified items.

	The copy operator return a reference to this item.

	The GetItemNo() function returns the item number which is
	the one used to sort the items in the array.

	The Get() returns a smart pointer to the property saved
	in this item.

BUGS

	This class is private mainly for safety. It would be nice
	if we could return an item to the end user to use directly.

SEE ALSO

	moPropArray class

*/
moPropArray::moItem::moItem(int item_no, const moProp *prop)
	: f_item_no(item_no),
	  f_prop(const_cast<moProp *>(prop))
{
}


moPropArray::moItem::~moItem()
{
}


const char *moPropArray::moItem::moGetClassName(void) const
{
	return "moBase::moProp::moPropArray.moBase::moItem";
}


moBase::compare_t moPropArray::moItem::Compare(const moBase& object) const
{
	const moItem& item = static_cast<const moItem&>(object);
	return CompareInt(f_item_no, item.f_item_no);
}


moPropArray::moItem& moPropArray::moItem::operator = (const moItem& item)
{
	if(f_item_no != item.f_item_no) {
		throw moError("moPropArray::moItem::operator = (): only expects items with the same item number.");
	}
	f_prop = item.f_prop;
	return *this;
}


int moPropArray::moItem::GetItemNo(void) const
{
	return f_item_no;
}


moPropSPtr moPropArray::moItem::Get(void) const
{
	return const_cast<moProp *>(static_cast<const moProp *>(f_prop));
}




/************************************************************ DOC:

CLASS

	moPropArray

NAME

	Constructor -- initialize an array
	Destructor -- cleans up the array
	moGetClassName -- returns the name of the class

SYNOPSIS

	moPropArray(mo_name_t name,
		moProp::prop_type_t type = moProp::MO_PROP_TYPE_UNKNOWN);
	virtual ~moPropArray();
	const char *moGetClassName(void) const;

PARAMETERS

	name - name of this property
	type - type of the array items (Defaults to 'any')

DESCRIPTION

	The array constructor expects the name of the property.

	Optionally, you can define a type which will be enforced
	each time you try to add an item in the array. Trying
	to add an item of the wrong type results in a throw.

RETURN VALUE

	The moGetClassName() function returns a read only static string with
	the name of this object class.

SEE ALSO

	Get, Set, Duplicate, Copy

*/
moPropArray::moPropArray(mo_name_t name, moProp::prop_type_t type)
	: moProp(name),
	  f_type(type)
{
}


moPropArray::~moPropArray()
{
}


const char *moPropArray::moGetClassName(void) const
{
	return "molib::moBase::moProp::moPropArray";
}


/************************************************************ DOC:

CLASS

	moPropArray

NAME

	GetType -- return MO_PROP_TYPE_ARRAY
	GetElementsType -- get the type of the elements

SYNOPSIS

	virtual prop_type_t GetType(void) const;
	prop_type_t GetElementsType(void) const;

DESCRIPTION

	The GetType() returns the type of this property which is
	MO_PROP_TYPE_ARRAY.

	The GetElementsType() returns MO_PROP_TYPE_UNKNOWN when
	the type of the elements wasn't specified (which means
	that the elements can be of any type) or the type which
	was specified on contruction of this array.

RETURN VALUE

	Both of these functions return an moProp::MO_PROP_TYPE_...

SEE ALSO

	Constructor

*/


/************************************************************ DOC:

CLASS

	moPropArray

NAME

	Copy -- copy an array in another
	Duplicate -- duplicate an exact copy of an array in another

SYNOPSIS

	virtual void Copy(const moProp& prop);
	virtual moPropSPtr Duplicate(void) const;

PARAMETERS

	prop - the array property to copy in this array

DESCRIPTION

	The Copy() function can be used to copy another
	array in this one. Note that the current array IS
	emptied before the copy occurs which means that the
	previous items are lost.

	The Duplicate() function allocates a new moArrayProp
	and copies this array in the new property. Finally,
	it returns the pointer to that new property.

	Note that the type of the new array that the Duplicate()
	function allocates is the same as this array type.

RETURN VALUE

	Duplicate returns a smart pointer to a newly allocated
	array which is a duplicate of this array.

ERRORS

	If the source array includes properties which aren't
	compatible with the destination array, then the Copy()
	instruction will throw an exception.

SEE ALSO

	Get, Set, Constructor

*/
void moPropArray::Copy(const moProp& prop)
{
	if(this == &prop) {
		return;
	}

	moProp::Copy(prop);

	// we want a complete overwrite
	Empty();

	const moPropArray& object = dynamic_cast<const moPropArray&>(prop);
	moList::position_t idx, max;
	if(f_type == MO_PROP_TYPE_UNKNOWN
	|| f_type == object.f_type) {
		f_array = object.f_array;
	}
	else {
		// in this case we want all the types to be checked
		max = object.f_array.Count();
		for(idx = 0; idx < max; ++idx) {
			moItem *item = object.f_array.Get(idx);
			Set(item->GetItemNo(), item->Get());
		}
	}
}


moPropSPtr moPropArray::Duplicate(void) const
{
	moPropSPtr array = new moPropArray(GetName(), f_type);
	array->Copy(*this);
	return array;
}


/************************************************************ DOC:

CLASS

	moPropArray

NAME

	Empty -- empty an array
	Get -- retrieve an item
	Set -- add or overwrite an item
	Delete -- delete an item

SYNOPSIS

	void Empty(void);
	moPropSPtr Get(int item_no) const;
	bool Set(int item_no, const moProp *prop);
	bool Delete(int item_no);

PARAMETERS

	item_no - the item number to be set, retrived or deleted
	prop - the property for that item number

DESCRIPTION

	The Empty() function will clear all the items in an array.

	The Get() function searches for the specified item number
	and return a pointer to its property.

	The Set() function creates a new item in the array or
	overwrites an existing item and sets that item property
	to the specified property.

	The Delete() function searches for the specified item number
	and deletes it.

	Note that all of these operations use the item number and
	not an index. This enables you to create arrays of items
	with holes and yet not use huge amounts of memory. The
	array can start and end at any number and the numbers can
	be really small or really large (they just need to fit in
	an integer).

	For instance, you can create the items 1 and 100. The array
	will then be composed of only two items and it won't use
	much memory at all. Trying to retrieve item 66 will give
	you a null pointer. Deleting item 1, makes the list shrink
	to having only one item left with item number 100.

	It is possible to retrieve, replace and delete items using
	their index in the array. The index in an array goes from
	0 to CountIndexes() - 1 and it is explained in with
	CountIndexes() and GetAtIndexs() functions.

RETURN VALUE

	Get() returns a property smart pointer which can be null
	when the specified item number doesn't exist in this array

	Set() returns true when it created a new item, false when
	it overwrote an existing item

	Delete() returns true when an item is really deleted,
	false when the specified item number isn't found

ERRORS

	If the source array includes properties which aren't
	compatible with the destination array, then the Copy()
	instruction will throw an exception.

SEE ALSO

	CountIndexes(), ItemNoAtIndex(), GetAtIndex(),
	SetAtIndex(), DeleteAtIndex()

*/
void moPropArray::Empty(void)
{
	f_array.Empty();
}


moPropSPtr moPropArray::Get(int item_no) const
{
	moItem item(item_no, 0);
	moList::position_t pos = f_array.Find(&item);
	if(pos == moList::NO_POSITION) {
		return 0;
	}
	return f_array.Get(pos)->Get();
}


bool moPropArray::Set(int item_no, const moProp *prop)
{
	if(f_type != MO_PROP_TYPE_UNKNOWN
	&& f_type != prop->GetType()) {
		throw moError("moPropArray::Set(): expected type %d, received %d", f_type, prop->GetType());
	}

	moItem item(item_no, prop);
	moList::position_t pos = f_array.Find(&item);
	if(pos == moList::NO_POSITION) {
		moItemSPtr item = new moItem(item_no, prop);
		f_array += item;
		return true;
	}

	*f_array.Get(pos) = item;

	return false;
}


bool moPropArray::Delete(int item_no)
{
	moItem item(item_no, 0);
	moList::position_t pos = f_array.Find(&item);
	if(pos != moList::NO_POSITION) {
		f_array.Delete(pos);
		return true;
	}

	return false;
}




/************************************************************ DOC:

CLASS

	moPropArray

NAME

	CountAtIndexes -- retrieve an item
	ItemNoAtIndex -- get the item number at the specified index
	GetAtIndex -- get an item at the specified index
	SetAtIndex -- replace an item at the specified index
	DeleteAtIndex -- delete an item at the specified index

SYNOPSIS

	unsigned long CountIndexes(void) const;
	int ItemNoAtIndex(int index) const;
	moPropSPtr GetAtIndex(int index) const;
	void SetAtIndex(int index, const moProp *prop);
	bool DeleteAtIndex(int index);

PARAMETERS

	index - the index to check out
	prop - the property used to replace an existing item property

DESCRIPTION

	Whenever you are working with an array, you have two sets
	of indexes. One is called the 'item number' (see Get(), Set()
	and Delete()) and the other is called 'index'.

	The functions which use the 'item number' have acces to
	a virtual array. The functions which use the 'index' can
	be used to enumate all the items available in an array
	and get their information, modify them or delete them.

	The CountIndexes() function returns the total number of
	items (and not the 'item number') currently available in
	the array. Items in the array can be accessed with an
	index defined between 0 and CountIndexes() - 1. If
	CountIndexes() returns 0, there are no items and you can't
	access anything.

	The ItemNoAtIndex() [i.e. 'item number at index'] searches
	for the specified item using its index and retrieve the
	'item number' of that item. Note that because all the values
	are valid for an item number, this function can't return
	any specific value if the specified index is outside the
	range of existing items. For this reason, it will throw an
	exception whenever the index is out of bounds.

	The GetAtIndex() function retrieves the property of the
	item using its index. This function can be used to enumerate
	all the items of the array when looping from 0 to
	CountIndexes() - 1 inclusive.

	The SetAtIndex() function replaces the property of the item
	specified at that index. The previous property is lost.
	Note that if the index is out of bounds, this function throws
	an exception. This is because otherwise the input property
	will be lost and it most certainly is an error.

	The DeleteAtIndex() function deletes the property at the
	specified index. Note that this function has the side
	effect of decreasing the CountIndexes() and there will
	be a new item at the same index if the index didn't
	specified the last item in the array.

RETURN VALUE

	CountIndexes() returns the total number of items in the array

	ItemNoAtIndex() returns the item number of the specified item

	GetAtIndex() returns the property of the specified item

	DeleteAtIndex() returns true when the specified item is deleted

ERRORS

	The ItemNoAtIndex() and SetAtIndex() functions will throw
	an exception on index overflow.

SEE ALSO

	Get(), Set(), Delete()

*/
unsigned long moPropArray::CountIndexes(void) const
{
	return f_array.Count();
}


int moPropArray::ItemNoAtIndex(int index) const
{
	if((uint32_t) index >= f_array.Count()) {
		// invalid index
		throw moError("moPropArray::Item(): trying to get an item with an invalid index of %d", index);
	}
	return f_array.Get(index)->GetItemNo();
}


moPropSPtr moPropArray::GetAtIndex(int index) const
{
	if((uint32_t) index >= f_array.Count()) {
		return 0;
	}
	return f_array.Get(index)->Get();
}


void moPropArray::SetAtIndex(int index, const moProp *prop)
{
	if((uint32_t) index >= f_array.Count()) {
		throw moError("moPropArray::Replace(): no item defined at index %d", index);
	}
	if(f_type != MO_PROP_TYPE_UNKNOWN
	&& f_type != prop->GetType()) {
		throw moError("moPropArray::SetAtIndex(): expected type %d, recevied %d", f_type, prop->GetType());
	}
	moItemSPtr item = f_array.Get(index);
	moItem new_item(item->GetItemNo(), prop);
	*item = new_item;
}


bool moPropArray::DeleteAtIndex(int index)
{
	if((uint32_t) index < f_array.Count()) {
		f_array.Delete(index);
		return true;
	}

	return false;
}












/************************************************************ DOC:

CLASS

	moPropRef

NAME

	Constructors - initialize a property reference
	Destructor - cleans up an moPropRef object

SYNOPSIS

	moPropRef(const moName& name);
	moPropRef(const moPropRef& prop_ref);
	moPropRef(const moName& name, const moPropRef& prop_ref);
	~moPropRef();

	protected:
	moPropRef(moPropBag *prop_bag, moProp *prop);

PARAMETERS

	name - name of this property
	prop_ref - the property to be referenced
	prop_bag - the bag this property is linked with
	prop - the property to be linked here

DESCRIPTION

	The moPropRef constructors will initialize a property reference
	using the specified name and property.

	The name can be any valid name as returned by the moNamePool
	object. Note that you can't use an index here.

	The prop_ref parameter can be any property of the same type of
	this property reference.

	Note that you can create a reference with a different name than
	the property being referenced.

	The protected constructor is only for bags to create property
	references as required. Users should not use it.

SEE ALSO

	IsNull, IsLinked, GetName, GetType, Link, GetLink

*/
moPropRef::moPropRef(const moName& name)
	: f_name(name)
{
}


moPropRef::moPropRef(const moPropRef& prop_ref)
	: f_name(prop_ref.f_name),
	  f_prop(prop_ref.f_prop)
{
}


moPropRef::moPropRef(const moName& name, const moPropRef& prop_ref)
	: f_name(name),
	  f_prop(prop_ref.f_prop)
{
}


moPropRef::moPropRef(const moPropBag *prop_bag, const moProp *prop)
	: f_name(prop->GetName()),
	  f_link(prop_bag),
	  f_prop(prop)
{
}


moPropRef::~moPropRef()
{
#if 0
printf("this = %p f_prop = %p, f_link %p\n",
		this,
		static_cast<moProp *>(f_prop),
		static_cast<moPropBag *>(f_link));
#endif
}


/************************************************************ DOC:

CLASS

	moPropRef

NAME

	GetName - return the name of this property
	GetType - return the type of this property
	IsNull - check whether the reference points to a real property

SYNOPSIS

	mo_name_t GetName(void) const;
	virtual moProp::prop_type_t GetType(void) const;
	bool IsNull(void) const;

DESCRIPTION

	The GetName() function returns the name of this property reference.
	The name of a property reference is always the same as the name
	of the property it references. The name is a unique number
	defined using the moNamePool object.

	Note that the name of a property or a property reference can't
	be changed at any time.

	The GetType() function returns the type of this property
	reference. The type of a property is hard coded for all
	properties and property references. For each property type
	there is a property reference. The types can't be mixed.

	The IsNull() function returns true whenever the property
	reference is NOT referencing a property. Note that a
	property reference may reference a property and yet not
	be linked.

SEE ALSO

	IsLinked, Link, GetLink

*/
moProp::prop_type_t moPropRef::GetType(void) const
{
	if(HasProp()) {
		return f_prop->GetType();
	}

	// if we're not linked yet, we can't know the type
	return moProp::MO_PROP_TYPE_UNKNOWN;
}


/************************************************************ DOC:

CLASS

	moPropRef

NAME

	Link - link this reference with a property bag
	IsLinked - check whether this reference is linked
	GetLink - get a pointer to the property bag this is linked to

SYNOPSIS

	void Link(const moPropBagRef prop_bag);
	bool IsLinked(void) const;
	moPropBagSPtr GetLink(void) const;
	bool HasProp(void) const;

PARAMETERS

	prop_bag - the property bag to which this property is to be linked to

DESCRIPTION

	The Link() function links this property reference to the specified
	property bag. This means changing the value of this property
	reference will then automatically be reflected in the property
	of the same name in the property bag.

	One can view this property reference, once linked, as a view
	inside the property bag being linked with it.

	Note that the current value of the reference is lost whenever the
	link succeeds.

	The IsLinked() function can be used to know whether the property
	reference is indeed linked with a property bag.

	The GetLink() function can be used to get the property bag pointer
	to which this property is linked. This is mainly a convenience
	since it should never really be necessary to get this pointer.

	The HasProp() function returns true if the property
	bag has a property defined. This means operation on the
	property will work as expected.

SEE ALSO

	IsNull, GetName, GetType

*/
void moPropRef::Link(const moPropBagRef prop_bag)
{
	if(f_link != prop_bag.f_prop) {
		f_link.Copy(prop_bag.f_prop);
//printf("Old prop %p (%ld)", static_cast<moProp *>(f_prop), f_prop->ReferenceCount());
		f_prop.Copy(f_link->Get(f_name));
//printf(" replaced by %p (%ld)\n", static_cast<moProp *>(f_prop), f_prop->ReferenceCount());
	}
}








/************************************************************ DOC:

CLASS

	moPropBagRef

NAME

	Constructors - initialize a property bag reference
	Destructor - cleans up the property bag reference object
	operator = - the assignment operator used to duplicate a property bag
	NewProp - create a new property bag in this reference
	Empty - empty the property bag

SYNOPSIS

	moPropBagRef(const moName& name);
	moPropBagRef(const moPropRef& prop_ref);
	moPropBagRef(const moName& name, const moPropRef& prop_ref);

	moPropBagRef(const moPropBag& prop_bag);
	moPropBagRef& operator = (const moPropBag& bag)
	moPropBagRef& operator = (const moPropBagRef& bag)

	void NewProp(void);
	void Empty(void);
	bool IsEmpty(void) const;
	unsigned long Count(void) const;

PARAMETERS

	name - the name of the new property bag
	prop_ref - the property being referenced
	prop_bag - a property bag to be duplicated

DESCRIPTION

	The property bag reference constructors will initialize a
	property reference with the specified name and property.

	The copy and assignment operators will copy the existing
	property bag to a new property bag and save the pointer
	of the new property bag in this reference. In case of
	the assignment, the previous property bag is simply
	released once the new bag is saved in the property bag
	reference.

	The NewProp() function creates a new property bag and
	assigns it to this property bag reference. The previous
	property bag is then released. The new bag will be empty.

	The Empty() function releases all the properties currently
	available in this bag. Afterward, the bag is empty. The
	property reference is still linked to that bag however.

	The IsEmpty() function checks whether the property bag has
	any properties defined. If so, it returns false. When the
	bag is currently empty, then it returns true.

	The Count() function returns the number of element currently
	available in this bag. If it returns 0, then the bag is
	empty. This can be used to enumerate the property bag. Please,
	see the Get() function for more information on how to enumerate
	a property bag.

NOTES

	If you write something such as:

		prop_bag = prop_bag;

	then the property bag is indeed duplicated and the new
	property bag pointer saved in 'prop_bag'. The previous
	property bag is then released.

SEE ALSO

	Get, Set, Delete, many operators, HasProp

*/
moPropBagRef::moPropBagRef(const moName& name)
	: moPropRef(name)
{
}


moPropBagRef::moPropBagRef(const moPropRef& prop_ref)
	: moPropRef(prop_ref)
{
}


moPropBagRef::moPropBagRef(const moName& name, const moPropRef& prop_ref)
	: moPropRef(name, prop_ref)
{
}


moPropBagRef::moPropBagRef(const moPropBag& prop_bag)
	: moPropRef(prop_bag.GetName())
{
	// now do the copy
	static_cast<void>(this->operator = (prop_bag));
}


moPropBagRef::~moPropBagRef()
{
}


moPropBagRef& moPropBagRef::operator = (const moPropBag& prop_bag)
{
	//f_prop.Copy(prop_bag.Duplicate());
	PropBag().Copy(prop_bag);
	if(f_link) {
		// the new property is not automatically linked
		// so we need to link it now
		f_link->Set(f_name, f_prop);
	}

	return *this;
}


moPropBagRef& moPropBagRef::operator = (const moPropBagRef& prop_bag)
{
#if 0
// making a copy in this case seems wrong since copying a reference
// in another means copying the pointer of what's referenced and
// not duplicating the object!?
	//f_prop.Copy(prop_bag.PropBag().Duplicate());
	PropBag().Copy(prop_bag.PropBag());
#else
	f_prop = prop_bag.f_prop;
#endif
	if(f_link) {
		// the new property is not automatically linked
		// so we need to link it now
		f_link->Set(f_name, f_prop);
	}

	return *this;
}


void moPropBagRef::NewProp(void)
{
	f_prop = new moPropBag(f_name);
	if(f_link) {
		// the new property is not automatically linked
		// so we need to link it now
		f_link->Set(f_name, f_prop);
	}
}


void moPropBagRef::Empty(void)
{
	PropBag().Empty();
}


bool moPropBagRef::IsEmpty(void) const
{
	return Count() == 0;
}


unsigned long moPropBagRef::Count(void) const
{
	return f_prop ? dynamic_cast<const moPropBag *>(static_cast<const moProp *>(f_prop))->Count() : 0;
}



/************************************************************ DOC:

CLASS

	moPropBagRef

NAME

	Dump - print a complete bag to stderr

SYNOPSIS

	void Dump(unsigned int flags = DUMP_FLAG_RECURSIVE, const char *message = 0) const;

PARAMETERS

	flags - a set of DUMP_FLAG_...
	message - a message to print at the start and end

DESCRIPTION

	This function will dump the property bag in stdout in a text
	format.

SEE ALSO

	?

*/
void moPropBagRef::Dump(unsigned int flags, const char *message) const
{
	PropBag().Dump(flags, message);
}





/************************************************************ DOC:

CLASS

	moPropBagRef

NAME

	Get - get a reference to a property from within the property bag
	Set - saves a property in this property bag
	operator [] - same as Get or Set depending on usage

SYNOPSIS

	moPropRef Get(const moIndexOrName& index_or_name) const;
	moPropRef operator [] (const moIndexOrName& index_or_name) const;
	void Set(const moIndexOrName& index_or_name, const moProp& prop);
	moPropRef operator [] (const moIndexOrName& index_or_name);

PARAMETERS

	index_or_name - the index or name of the property to get or set
	prop - the property to save in the referenced property

DESCRIPTION

	The Get() and constant [] operator will be used to read a
	property from a property bag. When the property doesn't
	exists, the function creates a null reference (i.e. the
	IsNull() function will return true); otherwise a reference
	is created pointing to the property found and linked to
	this property bag.

	The new reference is automatically linked to the property
	bag that this reference holds.

	The Get() used with an index instead of a name can be
	useful to enumerate all the properties of a property bag.
	Note that in this case you must make sure that you have
	the property type of reference pointer: an moPropRefSPtr.
	This is because the type can't be known in advance.

	The following code shows an example of enumeration:

		moPropBagRef bag;
		...
		max = bag.Count();
		for(int i = 0; i < max; ++i) {
			moPropRefSPtr p = bag.Get(i);
			switch(p->Type()) {
			case MO_PROP_TYPE_INT:
				...
				break;

			case ...

			}
		}

	The Set() and non constant [] operator will be used to
	write a property in the property bag. Contrary to the
	Get(), the Set() will not automatically link the property
	to the specified property bag. This allows you to copy
	a property from one bag to another without changing the
	source property link.

SEE ALSO

	Empty, Count, Delete, HasProp

*/
moPropRef moPropBagRef::Get(const moIndexOrName index_or_name) const
{
	return moPropRef(&PropBag(), PropBag().Get(index_or_name));
}


#if 0
moPropRef moPropBagRef::Get(moIndexOrName& index_or_name) const
{
	return moPropRef(&PropBag(), PropBag().Get(static_cast<const int>(index_or_name)));
}
#endif


moPropRef moPropBagRef::operator [] (const moIndexOrName& index_or_name) const
{
	return Get(index_or_name);
}


void moPropBagRef::Set(const moIndexOrName& index_or_name, const moPropRef& prop)
{
	PropBag().Set(index_or_name, prop.GetProperty());
}


moPropRef moPropBagRef::operator [] (const moIndexOrName& index_or_name)
{
	moPropSPtr prop = PropBag().Get(index_or_name);
	if(!prop) {
		throw moError("Property not found for [] operator");
	}
	return moPropRef(&PropBag(), prop);
}



/************************************************************ DOC:

CLASS

	moPropBagRef

NAME

	boolean operator - returns true if there is a property bag
	logical not operator - returns true if there is no property bag

SYNOPSIS

	operator bool (void) const;
	bool operator ! (void) const;

DESCRIPTION

	To know whether a property bag reference currently has
	some property defined in it.

	The boolean operator is similar to the IsAttached()
	function. It can be used in if() statements as in:

		if(my_ref) {
			// the property reference is linked
			...
		}

	The logical not operator returns the opposite of the
	boolean operator. For instance, if you just always
	want to have a property bag attached, you can write
	the following:

		if(!my_ref) {
			my_ref.NewProp();
		}

	After these two statements, you are sure to have a
	reference in your property bag reference object.

WARNING

	These functions don't test whether the property bag
	reference is linked to some other property bag. This
	is to know whether this specific property bag reference
	currently holds a property bag pointer.

SEE ALSO

	Empty, IsEmpty, Count, Delete, moPropRef::IsLinked

*/
moPropBagRef::operator bool (void) const
{
	return f_prop;
}

bool moPropBagRef::operator ! (void) const
{
	return !f_prop;
}



/************************************************************ DOC:

CLASS

	moPropBagRef

NAME

	operator + - duplicate and add a property or property bag
	operator += - add a property or property bag

SYNOPSIS

	void Add(const moPropBagRef& prop_bag, bool overwrite = true);
	void Add(const moPropRef& prop, bool overwrite = true);

	moPropBagRef& operator += (const moPropBagRef& prop_bag);
	moPropBagRef& operator += (const moPropRef& prop);
	moPropBagRef operator + (const moPropBagRef& prop_bag);
	moPropBagRef operator + (const moPropRef& prop);
	friend moPropBagRef operator + (const moPropRef& prop1, const moPropBagRef& prop2);
	friend moPropBagRef operator + (const moPropRef& prop1, const moPropRef& prop2);

	moPropBagRef& operator |= (const moPropBagRef& bag);
	moPropBagRef& operator |= (const moPropRef& prop);
	moPropBagRef operator | (const moPropBagRef& bag);
	moPropBagRef operator | (const moPropRef& prop);

PARAMETERS

	prop_bag - the property bag to add to this bag
	prop - the property to add to this bag
	prop1 - a property to add to a bag
	prop2 - a property to add to a bag
	overwrite - whether existing properties are overwritten

DESCRIPTION

	The + and += operators act as the Set() function by adding
	the given property or all the properties of a property bag
	to this property bag.

	The + operator first copies this property bag and then adds
	the properties to the copy. The source is not modified. Because
	a whole copy of the property bag is done, it is adviced to use
	this operator with caution.

	The + and += operators will overwrite existing properties
	with the value of the source properties. If you want to
	set default values and thus not overwrite existing values,
	use one of the | or |= operators instead.

	The special operator adding two properties together has
	a drawback: it names the new property bag "no-name" which
	may not be what you'd expect. This should be used whenever
	you need some temporary property bag for test purposes or
	a quick use.

BUGS

	The following operator:

		operator + (const moProp& prop1, const moProp& prop2);

	creates a property bag named "no-name".

SEE ALSO

	

*/
void moPropBagRef::Add(const moPropBagRef& prop_bag, bool overwrite)
{
	int		idx;
	moPropBag&	dst = PropBag();

	idx = prop_bag.Count();
	while(idx > 0) {
		--idx;
		moPropRef prop = prop_bag.Get(idx);
		dst.Set(prop.GetName(), prop.GetProperty(), overwrite);
	}
}


void moPropBagRef::Add(const moPropRef& prop, bool overwrite)
{
	PropBag().Set(prop.GetName(), prop.GetProperty(), overwrite);
}


moPropBagRef& moPropBagRef::operator += (const moPropBagRef& prop_bag)
{
	Add(prop_bag);

	return *this;
}


moPropBagRef& moPropBagRef::operator += (const moPropRef& prop)
{
	Add(prop);

	return *this;
}


moPropBagRef moPropBagRef::operator + (const moPropBagRef& prop_bag)
{
	moPropBagRef result(PropBag());

	result.Add(prop_bag);

	return result;
}


moPropBagRef moPropBagRef::operator + (const moPropRef& prop)
{
	moPropBagRef result(PropBag());

	result.Add(prop);

	return result;
}


moPropBagRef& moPropBagRef::operator |= (const moPropBagRef& prop_bag)
{
	Add(prop_bag, false);
	
	return *this;
}


moPropBagRef& moPropBagRef::operator |= (const moPropRef& prop)
{
	Add(prop, false);

	return *this;
}


moPropBagRef moPropBagRef::operator | (const moPropBagRef& prop_bag)
{
	moPropBagRef result(PropBag());

	result.Add(prop_bag, false);

	return result;
}


moPropBagRef moPropBagRef::operator | (const moPropRef& prop)
{
	moPropBagRef result(PropBag());

	result.Add(prop, false);

	return result;
}



/************************************************************ DOC:

CLASS

	moPropBagRef

NAME

	Delete - remove properties from a property bag
	operator - - duplicate and remove a property or property bag
	operator -= - remove a property or property bag

SYNOPSIS

	void Delete(const moIndexOrName& index_or_name);
	void Delete(const moPropBag& prop_bag);

	moPropBagRef& operator -= (const moIndexOrName& index_or_name);
	moPropBagRef& operator -= (const moPropBag& prop_bag);
	moPropBagRef operator - (const moIndexOrName& index_or_name);
	moPropBagRef operator - (const moPropBag& prop_bag);

	moPropBagRef& operator &= (const moPropBag& prop_bag);
	moPropBagRef operator & (const moPropBag& prop_bag);

PARAMETERS

	index_or_name - the index or name of the property to delete
	prop_bag - the list of properties to be removed

DESCRIPTION

	The Delete() functions will search for the specified properties
	and remove it from this property bag. You can remove a single
	property or remove all the properties found in another bag.

	Note that you can empty a property bag with a simple call as
	follow:

		foo.Delete(foo);

	However, this is not efficient. Use the Empty() function
	instead.

	The - and -= operators act like the Delete() functions by
	removing the specified properties.

	The - operator first copies this property bag and then removes
	the properties from the copy. The source is not modified. Because
	a whole copy of the property bag is done, it is adviced to use
	this operator with caution.

	All of these functions do nothing if you try to delete a
	property which doesn't exist in this property bag.

	The & and &= operators compute the intersection of two property
	bags. The result is all the properties which are common to
	both property bags. The other properties will be deleted.

SEE ALSO

	operator &, operator &=

*/
void moPropBagRef::Delete(const moIndexOrName& index_or_name)
{
	PropBag().Delete(index_or_name);
}


void moPropBagRef::Delete(const moPropBagRef& prop_bag)
{
	int	idx;

	idx = prop_bag.Count();
	while(idx > 0) {
		--idx;
		PropBag().Delete(prop_bag.Get(idx).GetName());
	}
}


moPropBagRef& moPropBagRef::operator -= (const moIndexOrName& index_or_name)
{
	Delete(index_or_name);

	return *this;
}


moPropBagRef& moPropBagRef::operator -= (const moPropBagRef& prop_bag)
{
	Delete(prop_bag);

	return *this;
}


moPropBagRef moPropBagRef::operator - (const moIndexOrName& index_or_name)
{
	moPropBagRef result(PropBag());

	result.Delete(index_or_name);

	return result;
}


moPropBagRef moPropBagRef::operator - (const moPropBagRef& bag)
{
	moPropBagRef result(PropBag());

	result.Delete(bag);

	return result;
}


moPropBagRef& moPropBagRef::operator &= (const moPropBagRef& prop_bag)
{
	int		idx;
	mo_name_t	name;

	// This operation is equivalent to testing whether our
	// properties exist in the prop_bag parameter property
	// bag; if not, then delete it

	idx = Count();
	while(idx > 0) {
		--idx;
		name = PropBag().Get(idx)->GetName();
		if(!prop_bag.Get(moIndexOrName(name)).IsNull()) {
			PropBag().Delete(idx);
		}
	}

	return *this;
}


moPropBagRef moPropBagRef::operator & (const moPropBagRef& prop_bag)
{
	moPropBagRef result(PropBag());

	result &= prop_bag;

	return result;
}





/************************************************************ DOC:

CLASS

	moPropArrayRef

NAME

	Constructors -- initializes an array property reference
	Assignment operators -- copy a reference or an array

SYNOPSIS

	moPropArrayRef(const moName& name);
	moPropArrayRef(const moPropRef& prop_ref);
	moPropArrayRef(const moName& name, const moPropRef& prop_ref);
	moPropArrayRef(const moPropArray& prop_array);
	moPropArrayRef& operator = (const moPropArrayRef& array_ref);
	moPropArrayRef& operator = (const moPropArray& array);

PARAMETERS

	name - the name of the new property
	prop_ref - a reference to a property which is an array
	prop_array - an array to be referenced by this object
	array_ref - an array reference to copy (the pointer only)
	array - an array to copy (the entire array is copied)

DESCRIPTION

	The constructors initialize an moPropArrayRef object.

	When you use one of the reference which accepts a prop_ref
	as a parameter, that property reference needs to be a
	reference to an array or the other functions will fail.

	The constructor with an array reference (prop_array) creates
	a reference to that specific property array. The property
	array is not duplicated.

	The assignment operators will just copy the property reference
	pointer (array_ref) or copy the entire array property in a
	new one saved in the reference (array).

	Note that if the source and destination arrays support a
	different type of elements, then an error will be thrown.

NOTES

	The type of the elements is defined in the source property
	array or when you create a new array (see the NewProp()
	function.)

RETURN VALUE

	The assignment operator return a reference to this.

SEE ALSO

	GetType, GetElementsType, Empty, Get, Set, assignment operator

*/
moPropArrayRef::moPropArrayRef(const moName& name)
	: moPropRef(name)
{
}

moPropArrayRef::moPropArrayRef(const moPropRef& prop_ref)
	: moPropRef(prop_ref)
{
}

moPropArrayRef::moPropArrayRef(const moName& name, const moPropRef& prop_ref)
	: moPropRef(name, prop_ref)
{
}

moPropArrayRef::moPropArrayRef(const moPropArray& prop_array)
	: moPropRef(0, &prop_array)
{
}

moPropArrayRef& moPropArrayRef::operator = (const moPropArrayRef& array_ref)
{
	f_prop = array_ref.f_prop;
	if(f_link) {
		// the new property is not automatically linked
		// so we need to link it now
		f_link->Set(f_name, f_prop);
	}

	return *this;
}


moPropArrayRef& moPropArrayRef::operator = (const moPropArray& array)
{
	PropArray().Copy(*static_cast<const moProp *>(&array));
	if(f_link) {
		// the new property is not automatically linked
		// so we need to link it now
		f_link->Set(f_name, f_prop);
	}

	return *this;
}





/************************************************************ DOC:

CLASS

	moPropArrayRef

NAME

	GetType -- retrieve the type of this reference
	GetElementsType -- retrieve the type of the elements

SYNOPSIS

	virtual moProp::prop_type_t GetType(void) const;
	moProp::prop_type_t GetElementsType(void) const;

DESCRIPTION

	The GetType() function returns that this property is
	an array.

	The GetElementsType() function returns the type of
	properties that this array accepts. The MO_PROP_TYPE_UNKNOWN
	will be used to define an array which can accept any type
	as items. The type is set when you create the array
	property (see the NewProp() function).

SEE ALSO

	NewProp(), Constructors

*/
moProp::prop_type_t moPropArrayRef::GetType(void) const
{
	return moProp::MO_PROP_TYPE_ARRAY;
}

moProp::prop_type_t moPropArrayRef::GetElementsType(void) const
{
	return PropArray().GetElementsType();
}




/************************************************************ DOC:

CLASS

	moPropArrayRef

NAME

	Empty -- remove all the elements from an array

SYNOPSIS

	void Empty(void);

DESCRIPTION

	The Empty() function deletes all the elements of an
	array. This is similar to a call to NewProp() except
	that the property pointer isn't changed and other
	references will see the new empty property.

SEE ALSO

	NewProp(), Constructors

*/
void moPropArrayRef::Empty(void)
{
	PropArray().Empty();
}



/************************************************************ DOC:

CLASS

	moPropArrayRef

NAME

	Delete -- delete an item from an array
	Get -- retrieve a property from an array
	Set -- save a property in an array
	operator [] -- retrieve a property from an array

SYNOPSIS

	bool Delete(int item_no);
	moPropSPtr Get(int item_no) const;
	bool Set(int item_no, const moProp *prop);
	bool Set(int item_no, const moPropRef& prop_ref);
	moPropSPtr operator [] (int item_no) const;

PARAMETERS

	item_no -- the item to read/write/delete
	prop -- the property to save at the specified item position
	prop_ref -- a reference to a property to save in the array

DESCRIPTION

	The Delete() function searches for the specified item number
	and when found, it removes it from the array. If you need to
	delete all the items, use Empty() instead.

	The Get() function and [] operator will be used to retrieve
	an item from an array. Both return a smart pointer to a
	property which you can read and modify as required.

	The Set() functions both save the specified property in
	the array. Note that the pointer of the property is saved
	in the array. The property is not duplicated.

RETURN VALUE

	Delete() returns true when an item is being deleted. It
	returns false when no item with the specified number was
	found.

	Get() returns a smart pointer to a property.

	operator [] returns a smart pointer to a property.

	Both Set() return true when the property was added (a new
	item was created) and false when the new property was
	saved in an existing item.

BUGS

	Note that the item number is not equal to the index number.
	You can have sparse item numbers, whereas index numbers start
	at 0 and go up to the number of items in an array minus 1
	(there are no defined indexes if there are no items).

SEE ALSO

	CountIndexes(), GetAtIndex(), SetAtIndex(), DeleteAtIndex()

*/
bool moPropArrayRef::Delete(int item_no)
{
	return PropArray().Delete(item_no);
}


moPropSPtr moPropArrayRef::Get(int item_no) const
{
	return PropArray().Get(item_no);
}


bool moPropArrayRef::Set(int item_no, const moProp *prop)
{
	return PropArray().Set(item_no, prop);
}


bool moPropArrayRef::Set(int item_no, const moPropRef& prop)
{
	return PropArray().Set(item_no, prop.GetProperty());
}


moPropSPtr moPropArrayRef::operator [] (int item_no) const
{
	return PropArray().Get(item_no);
}




/************************************************************ DOC:

CLASS

	moPropArrayRef

NAME

	CountIndexes -- return the number of items in an array
	ItemNoAtIndex -- return the item number at the specified index
	DeleteAtIndex -- delete an item from an array
	GetAtIndex -- retrieve a property from an array
	SetAtIndex -- save a property in an array

SYNOPSIS

	int CountIndexes(void) const;
	int ItemNoAtIndex(int index) const;
	bool DeleteAtIndex(int index);
	moPropSPtr GetAtIndex(int index) const;
	void SetAtIndex(int index, const moProp *prop);

PARAMETERS

	index -- the index to access in the array
	prop -- the property to save at the specified item position
	prop_ref -- a reference to a property to save in the array

DESCRIPTION

	These functions will be used by functions which need to
	enumerate all the elements of an array without using their
	item number. In some cases, this is the only method one will
	use to retrieve the properties defined in an array.

	The CountIndexes() function returns the total number of
	items currently defined in the array.

	The ItemNoAtIndex() function returns the item number of
	the item defined at the specified index. This item number
	can later be used to find the same item with the Get()
	function.

	The DeleteAtIndex() function deletes the item at the specifed
	index. Note that makes the array smaller and all the
	items which had a larger index now have their respective
	indexes minus one. Deleting an item doesn't change the
	item numbers, only their index.

	The GetAtIndex() function retrieves the property at the
	specified index in the array. This function and the
	CountIndexes() are very useful to enumerate all the
	items in an array.

	The SetAtIndex() function can be used to change the
	property of an item in an array using their index.

RETURN VALUE

	CountIndexes() returns the total number of items in an array.

	ItemNoAtIndex() returns the item number of the item specified
	by an index.

	DeleteAtIndex() returns true when an item is being deleted.
	It returns false when no item with the specified number was
	found.

	GetAtIndex() returns a smart pointer to a property.

BUGS

	Note that the indexes and the item numbers are different
	in that the item numbers can be sparse whereas the indexes
	go from 0 to the number of items minus 1. Items are used
	to access the entries by their usual number, whereas
	indexes are used to enumerate all the items in an array.

	The ItemNoAtIndex() and SetAtIndex() functions don't accept
	out of bounds indexes. If an invalid index is specified,
	then the function throws an exception.

SEE ALSO

	Get(), Set(), Delete()

*/
int moPropArrayRef::CountIndexes(void) const
{
	return PropArray().CountIndexes();
}


int moPropArrayRef::ItemNoAtIndex(int index) const
{
	return PropArray().ItemNoAtIndex(index);
}


bool moPropArrayRef::DeleteAtIndex(int index)
{
	return PropArray().DeleteAtIndex(index);
}


moPropSPtr moPropArrayRef::GetAtIndex(int index) const
{
	return PropArray().GetAtIndex(index);
}


void moPropArrayRef::SetAtIndex(int index, const moProp *prop)
{
	PropArray().SetAtIndex(index, prop);
}



/************************************************************ DOC:

CLASS

	moPropArrayRef

NAME

	NewProp -- create a new property

SYNOPSIS

	void NewProp(moProp::prop_type_t type = moProp::MO_PROP_TYPE_UNKNOWN);

PARAMETERS

	type -- the type that the items can be; defaults to any type

DESCRIPTION

	The NewProp() function allocates a new array property and saves
	its pointer in the array property reference.

	The type specified in this function is the type that the objects
	in the array can be defined as. For instance, if you use the
	moProp::MO_PROP_TYPE_INT as the type, adding a floating point
	or a long long will result in throwing an exception.

	The previous array property will automatically be released.
	If no one else had a reference on it, it will be freed.

SEE ALSO

	Empty()

*/
void moPropArrayRef::NewProp(moProp::prop_type_t type)
{
	f_prop = new moPropArray(f_name, type);
	if(f_link) {
		// the new property is not automatically linked
		// so we need to link it now
		f_link->Set(f_name, f_prop);
	}
}




/************************************************************ DOC:

CLASS

	moPropArrayRef

NAME

	PropArray -- retrieve a reference to the array property

SYNOPSIS

	private:
	moPropArray& PropArray(void) const;

DESCRIPTION

	This function dynamic casts the property reference to
	an array property which we can use in the other functions.

RETURN VALUE

	PropArray() returns a C++ reference to a property array

BUGS

	If the reference put in an moPropArrayRef object is not
	a reference to an array, this function will either crash
	or return a null reference. Using a null reference will
	result in a crash.

SEE ALSO

	PropBagRef::PropBag

*/
moPropArray& moPropArrayRef::PropArray(void) const
{
	return *const_cast<moPropArray *>(dynamic_cast<const moPropArray *>
					(static_cast<const moProp *>(f_prop)));
}


/************************************************************ DOC:

CLASS

	moPropIO

NAME

	Destructor -- virtual destructor
	moGetClassName -- return the name of this class

SYNOPSIS

	virtual ~moPropIO();
	virtual const char *moGetClassName(void) const;

DESCRIPTION

	the moPropIO() destructor cleans up the objects.

	The moGetClassName() function returns the name of this class.

RETURN VALUE

	The moGetClassName() returns a pointer to a constant static string
	with the name of the class.

SEE ALSO

	Load, Save

*/
moPropIO::~moPropIO()
{
}


const char *moPropIO::moGetClassName(void) const
{
	return "molib::moBase::moPropIO";
}




/************************************************************ DOC:

CLASS

	moPropIO

NAME

	SetInput - defines the input stream
	SetOutput - defines the output stream

SYNOPSIS

	void SetInput(moIStream *input_stream);
	void SetOutput(moOStream *output_stream);
	void SetSavePointers(bool save_pointers = false);

PARAMETERS

	input_stream - an input stream
	output_stream - an output stream
	save_pointers - whether pointers should be saved

DESCRIPTION

	The SetInput() and SetOutput() functions define the
	input and output stream pointers.

	Note that the same stream can be used for both, the input
	and the output if you create and moIOStream.

	The SetSavePointers() function can be used to request
	the InternalSave() to save pointer properties. By default,
	such pointers will be ignored since they usually won't be
	of any use on Load().

SEE ALSO

	Load, Save

*/
void moPropIO::SetInput(moIStream *input_stream)
{
	f_input = input_stream;
}


void moPropIO::SetOutput(moOStream *output_stream)
{
	f_output = output_stream;
}


void moPropIO::SetSavePointers(bool save_pointers)
{
	// note that by default we don't want to save pointers
	f_save_pointers = save_pointers;
}





/************************************************************ DOC:

CLASS

	moPropIO

NAME

	Load - loads properties from a file
	Save - saves properties to a file
	InternalLoad - the file format specific implementation of Load
	InternalSave - the file format specific implementation of Save

SYNOPSIS

	int Load(moPropBagRef& prop_bag);
	int Save(const moPropBagRef& prop_bag);

	protected:
	virtual int InternalLoad(moPropBagRef& prop_bag) = 0;
	virtual int InternalSave(const moPropBagRef& prop_bag) = 0;

PARAMETERS

	prop_bag - a bag of properties

DESCRIPTION

	These functions will be used to load and save all the
	properties of a property bag.

	When the Load() command is called, the existing properties
	in the bag passed as parameter will be overwritten. Those
	read from the file and which don't already exist in the
	property bag are simply added to it. The properties which
	already were in the bag and not in the file are kept as
	is. This allows you to have default properties if you wish.

	If the prop_bag parameter is a NULL reference, the Load()
	function will create a bag automatically.

	The input data the Load() function uses are read from
	the specified input stream.

	The Save() function will go through the tree of properties
	and save each one of them in the output stream. The
	way the tree is described in the output file is left to
	the end user. The Save() function only calls the
	InternalSave() if the property reference isn't a NULL
	pointer.

	The InternalLoad() and InternalSave() functions are to
	be derived by the classes which implement the actual
	loading and saving features.

	The property bag refrence will never be NULL whenever
	either of these functions are called.

	When an error occur in the InternalLoad() or the
	InternalSave() then these functions need to return -1.
	If everything works fine, return 0. The actual error
	should be marked using the SetError() function.

WARNING

	The InternalSave() has to honor the f_save_pointers flag.
	So, if that flag is true, pointers need to be saved in
	the output file and when it is false (the default) the
	pointers need to be skipped.

RETURN VALUE

	All of these functions will return 0 when nothing goes wrong.
	Otherwise they return -1. The error code can be retrieved
	using the GetLastError() function.

SEE ALSO

	SetInput, SetOutput, GetLastError, SetError

*/
int moPropIO::Load(moPropBagRef& prop_bag)
{
	// make sure there always is some property bag
	if(!prop_bag) {
		prop_bag.NewProp();
	}

	// call the user function
	if(InternalLoad(prop_bag)) {
		if(f_errno == 0) {
			// when no error was specified...
			SetError(MO_ERROR_INVALID);
		}
		return -1;
	}

	SetError(MO_ERROR_UNDEFINED);

	return 0;
}


int moPropIO::Save(const moPropBagRef& prop_bag)
{
	// try to save a NULL pointer fails
	if(!prop_bag) {
		SetError(MO_ERROR_EMPTY);
		return -1;
	}

	if(InternalSave(prop_bag)) {
		if(f_errno == 0) {
			// when no error was specified...
			SetError(MO_ERROR_INVALID);
		}
		return -1;
	}

	SetError(MO_ERROR_UNDEFINED);

	return 0;
}





/************************************************************ DOC:

CLASS

	moPropIO

NAME

	GetLastError - reads the last error code
	SetError - sets the current error code

SYNOPSIS

	int GetLastError(bool clear = true);

	protected:
	void SetError(int err);

PARAMETERS

	clear - whether to clear the error code after reading it

DESCRIPTION

	Whenever the Load() and Save() functions are called, these
	can generate error codes. In order to retrieve that error
	code, use the GetLastError() function. If you want to be
	able to call the function multiple times, make sure to use
	it with false as a parameter.

	The Load() and Save() functions will reset the error code
	if no error occured (i.e. the GetLastError() function will
	return 0 on successful calls.)

	The SetError() function is protected and can be used by
	derived classes to change the current error code.

RETURN VALUE

	The GetLastError() function returns the last error code.

SEE ALSO

	Load, Save

*/
int moPropIO::GetLastError(bool clear) const
{
	int	result;

	result = f_errno;

	if(clear) {
		f_errno = 0;
	}

	return result;
}


void moPropIO::SetError(int err)
{
	f_errno = err;
}




};		// namespace molib

// vim: ts=8 sw=8 syntax=cpp.doxygen

