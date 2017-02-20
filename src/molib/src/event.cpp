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
#pragma implementation "mo/mo_event.h"
#endif

#include	"mo/mo_event.h"

#ifdef MO_WIN32
#include <time.h>
#endif

namespace molib
{

#ifdef _MSC_VER
// avoid warnings about the time_t -> long conversions
#pragma warning(disable: 4244)
#endif



/************************************************************ DOC:

CLASS

	moEvent

NAME

	Constructors - initialize an moEvent object
	moGetClassName - return the name of this event class
	Assignment operator - copy an event in another
	Duplicate - create a duplicate of an event

SYNOPSIS

	moEvent(const moName& name, int order = 0);
	moEvent(const moEvent& event);
	virtual const char *moGetClassName(void) const;
	moEvent& operator = (const moEvent& event);
	moEventSPtr Duplicate(void) const;

PARAMETERS

	name - the name of the new event
	order - used to sort events in an event pipe
	event - event to copy

DESCRIPTION

	The moEvent object will in general be derived to create an
	event object to be posted in an moEventPipe.

	There are two constructors, one of which is a copy constructor.
	All events must have a name and they can also be given an
	order. The order is used to sort the events in the moEventPipe
	in which they are to be posted. Also, events can be given a
	time (see the SetTime() function). Events are actually sorted
	by time first, then by order for those events which have the
	same time.

	The assignment operator copies one event in another. The name
	order and time are copied. An event which has been posted will
	have been duplicated in the moEventPipe. Thus you are free to
	modify your event as much as you want.

	The Duplicate() function creates an exact duplicate of the event.
	This is used in the moEventPipe in order to make that the events
	in pipes are safe (i.e. you won't be able to modify them since
	you don't have their pointers) until retrieved by the pipe owner.

RETURN VALUE

	The moGetClassName() function returns a pointer to a constant
	static string with the name of the class (molib::moEvent).

	The assignment operator returns a reference to this.

	The Duplicate() function returns a smart pointer to an event
	object.

SEE ALSO

	moEvent class

*/
moEvent::moEvent(const moName& name, int order)
	: f_name(name),
	  f_order(order)
{
}


moEvent::moEvent(const moEvent& event)
	: moBase(event),
	  f_name(event.f_name),
	  f_order(event.f_order),
	  f_time(event.f_time),
	  f_dispatcher(event.f_dispatcher)
{
}


const char *moEvent::moGetClassName(void) const
{
	return "molib::moEvent";
}



moEvent& moEvent::operator = (const moEvent& event)
{
	if(this != &event) {
		moBase::operator = (event);

		f_name = event.f_name;
		f_order = event.f_order;
		f_time = event.f_time;
		f_dispatcher = event.f_dispatcher;
	}

	return *this;
}


moEventSPtr moEvent::Duplicate(void) const
{
	return new moEvent(*this);
}


/************************************************************ DOC:

CLASS

	moEvent

NAME

	GetName - get the name of the event
	SetOrder - change the numeric order
	GetOrder - get the current numeric order
	Compare - compare two events using their order

SYNOPSIS

	mo_name_t GetName(void) const;
	void SetOrder(int order);
	int GetOrder(void) const;
	void SetTime(time_t time);
	int GetTime(void) const;
	virtual compare_t Compare(const moBase& object) const;

PARAMETERS

	order - the numeric order
	object - another moEvent object

DESCRIPTION

	Each moEvent object can be assigned a name. Usually, the thread
	which will retrieve the moEvent via an moEventPipe will first
	check the name of the event to know how to handle it. The name
	will tell the user what event it is. For instance, when a
	thread dies, the moThread object sends a DEATH event. This is
	the name of the event. Once you know the name, you can cast the
	moEvent to the proper type to access the extraneous data available
	in the derived event. For instance, if you determine that the
	event is a DEATH event, you can cast it to moThread::moEventDeath
	as follow:

		moThread::moEventDeath *death;

		death = dynamic_cast<moThead::moEventDeath *>(event);

	Whenever you post an event, it is added in a list of events
	defined in an moEventPipe. In that list, the event pipe object
	sorts the objects using their order. This enables you to have
	out of bounds events, or very low priority events. For instance,
	to take a UI like example, you may want to first operate on
	mouse and keyboard events, then all the other events except
	events in link with drawing the screen which you want last. In
	this case, you have three levels:

		Mouse, Keyboard order is 1
		All other events 0
		Except Paint events which are -1

	Events can also be given a time. This means events for the
	future can be posted now and older events can be discarded.
	The time is specified with the SetTime() function call before
	the event is posted. Then, the different dispatch functions
	can take the time in account to know whether the event should
	be discarded (the event time is more than 1 second old),
	processed (the event time is between +/-1 second) or held in
	the queue (the event time is in the future). Since the time
	needs to be tested first, events are sorted by time before
	there are sorted by order. By default, events get a time of
	zero meaning that they are not affected by time. Only events
	which are posted in the future or those which can be discarded
	(i.e. these events you do not have to receive) should be given
	a time. It is very important to understand that a timed event
	may never arrive and thus you cannot count on it to be
	dispatched at some point.

	The Compare() function is automatically used by the
	moSortedList of the moEventPipe to order the events using
	their order number.

NOTES

	Timed events may be changed later to allow posts of future events
	which are ensured to be received even if late when checked out.

RETURN VALUE

	The GetName() function returns a name as moNamePool generates

	The GetOrder() function returns the order of this event

SEE ALSO

	moEventPipe class

*/
mo_name_t moEvent::GetName(void) const
{
	return f_name;
}


void moEvent::SetOrder(int order)
{
	f_order = order;
}


int moEvent::GetOrder(void) const
{
	return f_order;
}


void moEvent::SetTime(time_t time)
{
	f_time = time;
}


time_t moEvent::GetTime(void) const
{
	return f_time;
}


moBase::compare_t moEvent::Compare(const moBase& object) const
{
	compare_t r;
	const moEvent& event = dynamic_cast<const moEvent&>(object);

	// first sort by time, then by order
	r = CompareInt(f_time, event.f_time);
	if(r == MO_BASE_COMPARE_EQUAL) {
		r = CompareInt(f_order, event.f_order);
	}

	return r;
}






/************************************************************ DOC:

CLASS

	moEvent

NAME

	SetDispatcher - object to be called when this event is dispatched
	Dispatch - dispatch the event

SYNOPSIS

	void SetDispatcher(const moEventDispatcher *dispatcher);
	void Dispatch(void);

PARAMETERS

	dispatcher - an object derived from the moEventDispatcher class

DESCRIPTION

	If you want to beneficiate of the automatic dispatching system
	then you will need to add a dispatcher on every single event
	you create.

	To do so, use the SetDispatcher() function on the event when
	the dispatcher pointer to call at the time the event is being
	retrieved from the pipe.

	One can call the Dispatch() function to execute the
	EventDispatch() of the moDispatcher class which is currently
	attached to this event.

SEE ALSO

	moEventPipe class

*/
void moEvent::SetDispatcher(const moEventDispatcher *dispatcher)
{
	f_dispatcher = dispatcher;
}


void moEvent::Dispatch(void)
{
	if(f_dispatcher) {
		f_dispatcher->EventDispatch(*this);
	}
}







/************************************************************ DOC:

CLASS

	moEventPipe

NAME

	moGetClassName - get the name of this class

SYNOPSIS

	virtual const char *moGetClassName(void) const;

DESCRIPTION

	Please, see moBase::moGetClassName() for more information.

RETURN VALUE

	The pointer to the name of this class in a static string.

SEE ALSO

	moBase::moGetClassName() function

*/
const char *moEventPipe::moGetClassName(void) const
{
	return "molib::moEventPipe";
}



/************************************************************ DOC:

CLASS

	moEventPipe

NAME

	Post - post an event in this event pipe
	Next - wait until an event is available and return it
	Peek - get the next event if any, or return false
	Wait - wait until an event is available

SYNOPSIS

	void Post(moEvent& event);
	void Flush(void);
	bool Next(moEventSPtr& event_ptr);
	bool Peek(moEventSPtr& event_ptr, bool remove = false);
	void Wait(void) const;

PARAMETERS

	event - the event to post
	event_ptr - a pointer where the event being read is returned
	remove - whether to remove the event from the pipe

DESCRIPTION

	The moEventPipe object will be used by one thread to receive
	events from other threads. Any thread, including the owner of
	the event pipe can send an event in the pipe using the Post()
	function.

	Note that timed events which have a date in the past can be
	posted. However, nothing will be put in the pipe in this case
	since the event will already be considered too old. This means
	you should date your event right before to post it rather than
	when you create the event (it depends how long it takes your
	code to create the event, that is).

	The owner of the pipe (and none other in most cases) can either
	use the Next() or the Peek() functions to retrieve the next
	event. The Next() will block until an event is posted in the
	moEventPipe. The Peek() won't block if there isn't any message
	and also it will leave the message in the pipe unless the
	remove flag is set to true (note that the default is false).

	The Next() and Peek() with the remove flag set to true will
	dispatch the event if the SetAutoDispatch() function was called
	with true as a parameter. The two functions still return the
	event which could have been modified by the dispatch function.
	See also the moEvent::SetDispatcher() function for more
	information.

	If your thread wants to wait until an event is available, then
	use the Wait() function. That function will block until there
	is at least one event in the event pipe. This is the sole
	purpose of that function. It can be useful in case you don't
	need to know what the event is until later in your function.

	In all cases, the functions will check timed events as expected.
	This means events of the past are automatically discarded and
	events for the future are ignored. In other words, the Peek()
	function will return no event if the pipe isn't empty but only
	includes events which are to be dispatched in the future.

	The Next() and Wait() functions are optimized to wait until
	the date of the next event in the future is reached or a new
	message arrives.

	The Flush() can be used to empty the pipe entirely. Be careful
	as some events may need to be received otherwise the behavior
	of your software may be compromised.

NOTES

	It is unlikely useful, but an moEventPipe can also be owned
	by multiple threads. If these are worker threads, then an
	event can be sent to any one of them for execution of some
	task. The main problem here is that you can't be sure which
	thread will receive which event.

	The multi-threading synchronization are archived using the
	moMutex object.

RETURN VALUE

	The Peek() function returns true if an event was available

BUGS

	The Peek() function will also return false if it can't get a
	lock to look at the pipe data. Yet, the pipe may not be empty.

SEE ALSO

	moEvent class

*/
void moEventPipe::Post(moEvent& event)
{
	// worth posting?!
	time_t t = event.GetTime();
	if(t != 0 && t - time(NULL) < -1) {
		return;
	}

	moEventSPtr n = event.Duplicate();

	moLockMutex lock(f_mutex);

	f_pipe += *static_cast<moEvent *>(n);

	f_mutex.Signal();
}


void moEventPipe::Flush(void)
{
	moLockMutex lock(f_mutex);
	f_pipe.Empty();
}


void moEventPipe::Next(moEventSPtr& event)
{
	f_mutex.Lock();
	for(;;) {
		if(!f_pipe.IsEmpty()) {
			event = f_pipe.GetFirst();
			time_t t, d;
			t = d = event->GetTime();
			if(t != 0) {
				t -= time(NULL);
			}
			if(labs(t) <= 1) {
				// event with the right time
				f_pipe.Delete(0);
				f_mutex.Unlock();
				if(f_auto_dispatch) {
					event->Dispatch();
				}
				return;
			}
			if(t < 0) {
				// too old an event, discard
				// and try the next one
				f_pipe.Delete(0);
				continue;
			}
			// otherwise, that's an event in the future,
			// we've got to sleep! So we will be waiting
			// until either a new event arrives or this
			// event in the future needs to be posted
			f_mutex.DatedWait(static_cast<unsigned long long>(d) * 1000ULL);
		}
		else {
			f_mutex.Wait();
		}
	}
}


bool moEventPipe::Peek(moEventSPtr& event, bool remove)
{
	if(f_mutex.TryLock()) {
		while(!f_pipe.IsEmpty()) {
			event = f_pipe.GetFirst();
			time_t t = event->GetTime();
			if(t != 0) {
				t -= time(NULL);
			}
			if(labs(t) <= 1) {
				// we can execute this message
				if(remove) {
					f_pipe.Delete(0);
					if(f_auto_dispatch) {
						f_mutex.Unlock();
						event->Dispatch();
						return true;
					}
				}
				f_mutex.Unlock();
				return true;
			}
			if(t < 0) {
				// too old an event, discard!
				// try with the next event if any
				f_pipe.Delete(0);
			}
			else {
				// nothing to do yet...
				break;
			}
		}
		f_mutex.Unlock();
	}

	event = 0;

	return false;
}


void moEventPipe::Wait(void) const
{
	f_mutex.Lock();
	for(;;) {
		if(!f_pipe.IsEmpty()) {
			time_t t, d;
			t = d = f_pipe.GetFirst()->GetTime();
			if(t != 0) {
				t -= time(NULL);
			}
			if(labs(t) <= 1) {
				f_mutex.Unlock();
				return;
			}
			if(t < 0) {
				// discard old messages
				const_cast<moSortedListOfEvents&>(f_pipe).Delete(0);
			}
			else {
				// wait for either a new message or this
				// one time to be reached
				f_mutex.DatedWait(static_cast<unsigned long long>(d) * 1000ULL);
			}
		}
		else {
			// wait for a new message
			f_mutex.Wait();
		}
	}
}



/************************************************************ DOC:

CLASS

	moEventPipe

NAME

	GetAutoDispatch - check the current auto-dispatch flag setup
	SetAutoDispatch - set the auto-dispatch flag as required

SYNOPSIS

	bool GetAutoDispatch(void) const;
	void SetAutoDispatch(bool auto_dispatch = true);

PARAMETERS

	auto_dispatch - whether to auto-dispatch or not

DESCRIPTION

	Whenever a message is taken out of an moEventPipe, you can
	request that pipe to automatically call the dispatch function
	defined on the moEvent.

	The Next() function will always call the dispatch function.

	The Peek() function calls the dispatch function only if the
	remove flag is set to true.

	By default, the events are not automatically dispatched.

RETURN VALUE

	The GetAutoDispatch() function returns true if events are
	to be dispatched

SEE ALSO

	moEvent class

*/
bool moEventPipe::GetAutoDispatch(void) const
{
	return f_auto_dispatch;
}


void moEventPipe::SetAutoDispatch(bool auto_dispatch)
{
	f_auto_dispatch = auto_dispatch;
}





/** \brief Read the next event from the event pipe.
 *
 * This function reads and removes one event from the pipe and
 * dispatches it.
 *
 * This means the corresponding receivers functions are called
 * and the event will not be available in the event pipe anymore.
 *
 * \param[out] event The smart pointer where the event is saved.
 *
 * \sa moEventPipeBroadcast::Peek(moEventSPtr& event, bool remove)
 * \sa moEventPipeBroadcast::Dispatch(moEvent *event)
 */
void moEventPipeBroadcast::Next(moEventSPtr& event)
{
	// get the event from the pipe
	moEventPipe::Next(event);
	Dispatch(event);
}

/** \brief Read the next event from the event pipe.
 *
 * This function reads the next event in the event pipe.
 *
 * By default, the event is kept in the pipe (i.e. remove is set to false.)
 *
 * When the event is removed from the pipe, then this function
 * also dispatches it before it returns. This is similar to
 * calling the function Next(moEventSPtr& event).
 *
 * If there is an event, the function returns true.
 *
 * \param[out] event The smart pointer where the event is saved.
 * \param[in] remove Remove the event from the pipe.
 *
 * \return true if an event is available, false otherwise
 *
 * \sa moEventPipeBroadcast::Next(moEventSPtr& event)
 * \sa moEventPipeBroadcast::Dispatch(moEvent *event)
 */
bool moEventPipeBroadcast::Peek(moEventSPtr& event, bool remove)
{
	if(moEventPipe::Peek(event, remove)) {
		if(remove) {
			Dispatch(event);
		}
		return true;
	}

	return false;
}






/************************************************************ DOC:

CLASS

	moEventPipeBroadcast

NAME

	AddReceiver - add a receiver for any event posted on this pipe
	RemoveReceiver - remove a receiver previously added to this pipe
	Execute - process the event with the different receivers

SYNOPSIS

	void AddReceiver(const moEventReceiver *receiver);
	void RemoveReceiver(const moEventReceiver *receiver);
	void Dispatch(moEvent *event);

PARAMETERS

	receive - a pointer to a valid receiver
	event - the event to process using the receivers

DESCRIPTION

	The AddReceiver() function will add the given receiver
	pointer to the list of objects to receive events. Note
	that a receiver will receive all the events, though most
	will usually be ignored. You can insert the same receiver
	multiple times, but watch out, if you do so, your functions
	will be executed multiple times too.

	The RemoveReceiver() function searches for a previously
	added receiver and remove the first instance it finds. If
	the same receiver had been added multiple times, then it
	has to be removed multiple times to be removed completely.

	The Dispatch() function takes an event as a parameter and
	sends that event to each of the receivers currently attached
	to the moEventPipeBroadcast object. This is usually called
	internally by the Next() and Peek() functions whenever they
	retrieve an event, but one may want to trigger an event
	right away. (warning: this takes precedence over all the
	events currently in the pipe and it is not validated in
	any strong way like the other events).

RETURN VALUE

	The Peek() function returns true whenever an event was
	available, false otherwise.

SEE ALSO

	moEvent::Next, moEvent::Peek

*/
/** \brief Add a receiver to this pipe.
 *
 * This function adds a receiver to this event pipe. Whenever an event is
 * dispatched, the corresponding function of all the receivers added in
 * this way are called.
 *
 * \note
 * It is possible to add the same receiver multiple times, but that means
 * your functions will be called that many times. Also, you need to remove
 * it the same number of times to completely remove it from the list of
 * receivers.
 *
 * \param[in] receiver The receiver to add to this pipe.
 */
void moEventPipeBroadcast::AddReceiver(const moEventReceiver *receiver)
{
	moLockMutex lock(f_mutex);

	f_receivers += *receiver;
}

/** \brief Remove a receiver previously added with AddReceiver.
 *
 * This function searches the list of receivers and removes the first
 * instance it finds.
 *
 * \bug
 * Notice that the RemoveReceiver() will first remove the very first
 * instance of a receiver that was added with AddReceiver(). However,
 * it is not expected to be a problem since it is quite unlikely that
 * you will need to add the same receiver multiple times to the same
 * pipe.
 *
 * \param[in] receiver The receiver to remove from this pipe.
 */
void moEventPipeBroadcast::RemoveReceiver(const moEventReceiver *receiver)
{
	moList::position_t	idx, max;

	moLockMutex lock(f_mutex);

	max = f_receivers.Count();
	for(idx = 0; idx < max; ++idx) {
		if(f_receivers.Get(idx) == receiver) {
			f_receivers.Delete(idx);
			return;
		}
	}
}


/** \brief Remove ALL receivers previously added with AddReceiver.
 *
 * This function purges the whole list of receivers previously with
 * AddReceiver.
 *
 * \sa AddReceiver, RemoveReceiver
 */
void moEventPipeBroadcast::RemoveAllReceivers(void)
{
	moLockMutex lock( f_mutex );

	while( f_receivers.Count() > 0 )
	{
	    f_receivers.Delete( 0 );
	}
}


/** \brief Dispatch events to all interested receivers
 *
 * This method sends events to all interestes receivers in the current
 * receivers list.
 *
 * \param[in] event to dispatch
 * 
 * \sa moEventReceiver::SendToReceivers
 */
void moEventPipeBroadcast::Dispatch(moEvent *event)
{
	moReceiversEvent	*receiver;
	moSortedList		receivers;

	receiver = dynamic_cast<moReceiversEvent *>(event);
	if(receiver == 0) {
		return;
	}

	// NOTE: I'm not too sure we want to keep this; it prevents
	//	 a long lock (and many possible dead locks) but gee's!
	//	 it will copy hundreds of pointers all the time...
	//	 another solution is to have a special mutex for the
	//	 management of the receivers list, and possibly two
	//	 lists one to add/remove and one to dispatch (very
	//	 much like the list of tasks in the task manager)
	{
		moLockMutex lock(f_mutex);
		// make a copy because we are in a multi-threading
		// environment and we don't want to lock the pipe
		// for the whole time it is executing events!
		receivers = f_receivers;
	}

	receiver->SendToReceivers(receivers);
}





/************************************************************ DOC:

CLASS

	moEventReceiver

NAME

	GetPriority - returns the current priority of a receiver
	SetPriority - sets the priority of a receiver

SYNOPSIS

	void SetPriority(int priority);
	int GetPriority(void) const;

PARAMETERS

	priority - the new priority for a receiver

DESCRIPTION

	The receivers will be sorted using their priority. In most
	cases, receivers will not change their priority and they
	will be executed in an unspecified order.

	The receiver with the largest priority will always be called
	first. Receivers with equal priority are called in any order.

RETURN VALUE

	The GetPriority() function returns the current priority of
	a receiver.

SEE ALSO

	moEventPipeBroadcast class

*/
void moEventReceiver::SetPriority(int priority)
{
	f_priority = priority;
}


int moEventReceiver::GetPriority(void) const
{
	return f_priority;
}


/************************************************************ DOC:

CLASS

	moEventReceiver

NAME

	Compare - compares the priority of two receivers

SYNOPSIS

	compare_t Compare(const moBase& object) const;

PARAMETERS

	object - the other receiver to compare priority with

DESCRIPTION

	When receivers are ready to have some of their functions
	called, they are copied in a sorted list. This list is
	sorted according to their current priority.

RETURN VALUE

	One of the MO_BASE_COMPARE_... values

SEE ALSO

	SetPriority(), GetPriority()

*/
moBase::compare_t moEventReceiver::Compare(const moBase& object) const
{
	return CompareInt(dynamic_cast<const moEventReceiver&>(object).f_priority, f_priority);
}




/************************************************************ DOC:

CLASS

	moReceiversEvent

NAME

	Constructors - to initialize a receivers event

SYNOPSIS

	moReceiversEvent(const moName& name, int order = 0);
	moReceiversEvent(const moEvent& event);

PARAMETERS

	name - the name of the event
	order - the order to sort events among each others
	event - an event to copy

DESCRIPTION

	The receivers events are special events which can be
	used with the moEventPipeBroadcast object.

	Whenever such an event is used with such a pipe, its
	SendToReceivers() function is automatically called
	whenever an event is retrieved from the pipe (see the
	Next and Peek functions of the moEventPipeBroadcast).

SEE ALSO

	moEventPipeBoardcast::Next
	moEventPipeBoardcast::Peek

*/
moReceiversEvent::moReceiversEvent(const moName& name, int order)
	: moEvent(name, order)
{
}

moReceiversEvent::moReceiversEvent(const moEvent& event)
	: moEvent(event)
{
}



/************************************************************ DOC:

CLASS

	moReceiversEvent

NAME

	Duplicate - allocate a copy of this event

SYNOPSIS

	virtual moEventSPtr Duplicate(void) const = 0;

DESCRIPTION

	You need to have your own Duplicate() function because you
	need to have your SendToReceivers() function called whenever
	an event is received.

	Whenever an event is being posted, it needs to be duplicated.
	This function is called automatically by the moEventPipe
	whenever Post() is called. This is important because several
	threads are using the different objects and often the user
	will create his events on the stack and that event needs to
	still be present in the pipe whenever the other thread receives
	it.

SEE ALSO

	SendToReceivers

*/
#if 0
moEventSPtr moReceiversEvent::Duplicate(void) const
{
	return new moReceiversEvent(*this);
}
#endif



/************************************************************ DOC:

CLASS

	moReceiversEvent

NAME

	SendToReceivers - to send an event whenever the pipe gets one

SYNOPSIS

	virutal void SendToReceivers(const moSortedList& receivers) = 0;

DESCRIPTION

	This function will send the event to all the receivers mentioned
	in the list of receivers. It is a pure virtual here since we
	can't know of the functions to be called in the lower level
	implementation (these are not declared here yet!).

SEE ALSO

	Duplicate, moEventPipeBroadcast::Dispatch

*/







};			// namespace molib;

// vim: ts=8

