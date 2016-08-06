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



#ifndef MO_MUTEX_H
#define	MO_MUTEX_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_BASE_H
#include	"mo_base.h"
#endif


namespace molib
{



// IMPORTANT NOTE: moMutex is on purpose NOT derived of moBase
// this enables the moBase object to actually use the mutex object
class MO_DLL_EXPORT moMutex
{
public:
				moMutex(void);
				~moMutex();

	void			Lock(void);
	bool			TryLock(void);
	void			Unlock(void);
	void			Wait(void);
	bool			TimedWait(unsigned long long usec);
	bool			DatedWait(unsigned long long msec);
	void			Signal(void);

private:
	mutable muint32_t	f_reference_count;
#ifdef MO_THREAD
	pthread_mutex_t		f_mutex;
	pthread_cond_t		f_cond;
#endif
};





// This class is to create a temporary lock with a mutex
// thus we really don't need to have it derived from moBase
// Example:
//	foo()
//	{
//		moLockMutex autolock(my_mutex);
//
//		...	// single threaded work
//	}
class MO_DLL_EXPORT moLockMutex
{
public:
				moLockMutex(moMutex& mutex) : f_mutex(&mutex) { f_mutex->Lock(); }
				~moLockMutex() { Unlock(); }

	// premature unlocking of the mutex (before the '}')
	void			Unlock(void) { if(f_mutex) { f_mutex->Unlock(); f_mutex = 0; } }

private:
	moMutex *		f_mutex;
};




// The moSignal hides the need for a lock + unlock while waiting on a mutex
class MO_DLL_EXPORT moSignal : private moMutex
{
public:
	void			Wait(void);
	bool			TimedWait(unsigned long long usec);
	bool			DatedWait(unsigned long long msec);
	void			Signal(void);
};



};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_MUTEX_H

