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



#ifndef MO_EVENT_H
#define	MO_EVENT_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_NAME_H
#include	"mo_name.h"
#endif


namespace molib
{


class MO_DLL_EXPORT moEvent;

typedef moSmartPtr<moEvent>	moEventSPtr;


// when defined in an event, automatically called when Dispatch() is called
class MO_DLL_EXPORT moEventDispatcher : public moBase
{
public:
	// This is called whenever the moEvent::Dispatch() function
	// is called; the event is passed as a parameter
	virtual void		EventDispatch(moEvent& event) = 0;
};

typedef moSmartPtr<moEventDispatcher>	moEventDispatcherSPtr;




// an event, if you need specialized data, then derive from this class
class MO_DLL_EXPORT moEvent : public moBase
{
public:
				moEvent(const moName& name, int order = 0);
				moEvent(const moEvent& event);

	virtual const char *	moGetClassName(void) const;

	moEvent&		operator = (const moEvent& event);

	virtual moEventSPtr	Duplicate(void) const;

	mo_name_t		GetName(void) const;
	void			SetOrder(int order);
	int			GetOrder(void) const;
	void			SetTime(time_t time);
	time_t			GetTime(void) const;

	virtual compare_t	Compare(const moBase& object) const;

	void			SetDispatcher(const moEventDispatcher *dispatcher);
	void			Dispatch(void);

private:
	mo_name_t		f_name;
	zint32_t		f_order;
	ztime_t			f_time;
	moEventDispatcherSPtr	f_dispatcher;
};

typedef moTmplList<moEvent, moSortedList>	moSortedListOfEvents;






// an event pipe is a multithread safe FIFO of events
class MO_DLL_EXPORT moEventPipe : public moBase
{
public:
	virtual const char *	moGetClassName(void) const;

	virtual void		Post(moEvent& event);
	virtual void		Flush(void);
	virtual void		Next(moEventSPtr& event);
	virtual bool		Peek(moEventSPtr& event, bool remove = false);
	virtual void		Wait(void) const;

	bool			GetAutoDispatch(void) const;
	void			SetAutoDispatch(bool auto_dispatch = true);

protected:
	mutable moMutex		f_mutex;

private:
	moSortedListOfEvents	f_pipe;
	zbool_t			f_auto_dispatch;
};

typedef moSmartPtr<moEventPipe>		moEventPipeSPtr;
typedef moTmplList<moEventPipe, moList>	moListOfEventPipes;





class MO_DLL_EXPORT moReceiversEvent : public moEvent
{
public:
				moReceiversEvent(const moName& name, int order = 0);
				moReceiversEvent(const moEvent& event);

	// This function is already defined in moEvent so I don't think we
	// can later make it a pure virtual but that's very much the way
	// it should look here!
	//virtual moEventSPtr	Duplicate(void) const = 0;

	virtual void		SendToReceivers(const moSortedList& receivers) = 0;
};


class MO_DLL_EXPORT moEventReceiver : public moBase
{
public:
	void			SetPriority(int priority);
	int			GetPriority(void) const;

	virtual compare_t	Compare(const moBase& object) const;

private:
	zint32_t		f_priority;
};


class MO_DLL_EXPORT moEventPipeBroadcast : public moEventPipe
{
public:
	virtual void		Next(moEventSPtr& event);
	virtual bool		Peek(moEventSPtr& event, bool remove = false);

	void			AddReceiver(const moEventReceiver *receiver);
	void			RemoveReceiver(const moEventReceiver *receiver);
	void			RemoveAllReceivers(void);
	void			Dispatch(moEvent *event);

private:
	moList			f_receivers;
};




};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_EVENT_H

