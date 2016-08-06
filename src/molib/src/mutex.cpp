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
#pragma implementation "mo/mo_mutex.h"
#endif

#include	"mo/mo_mutex.h"

#ifdef WIN32
#include	<sys/types.h>
#include	<sys/timeb.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif


namespace molib
{



/************************************************************ DOC:

CLASS

	moMutex

NAME

	Constructor - initialize an moMutex object
	Destructor - cleans up an moMutex object

SYNOPSIS

	moMutex(void);
	~moMutex();

DESCRIPTION

	The moMutex object can be used to lock a part of the code
	that only one process should run at a time. This is at
	times called a critical section.

	One can use the moLockMutex object to temporarilly lock a
	mutex as follow:

		{
			moLockMutex lock(&my_mutex);
			...	// only one thread running
		}	// now the lock is over

	The lock can be tried meaning that if another thread already
	has the lock, it fails. See the TryLock() function.

NOTES

	There are currently three supported versions of the mutex:

	1. No multi-thread environment; this case is when the compilation
	   says that the multi-thread library won't be used

	2. Multi-thread using the pthread standard (Linux, Mac OS/X, IRIX)

	3. Multi-thread using the BeginThread() standard (whatever that is,
	   it's the one used in Microsoft Windows).

SEE ALSO

	Lock(), TryLock(), Unlock(), Wait(), TimedWait(),
	DatedWait(), Signal()

BUGS

	Care must be used to always initialized a mutex before it
	is possibly accessed by more than one thread. This is usually
	the case in the constructor of an object (and that's safe
	unless your constructor links your object in a list accessible
	by other threads prior to the mutex initialization, but who
	does that, really?).

*/
moMutex::moMutex(void)
	: f_reference_count(0)
{
#ifdef MO_THREAD
	pthread_mutexattr_t mattr;
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&f_mutex, &mattr);
	pthread_mutexattr_destroy(&mattr);

	pthread_condattr_t cattr;
	pthread_condattr_init(&cattr);
	pthread_cond_init(&f_cond, &cattr);
	pthread_condattr_destroy(&cattr);
#endif
}

moMutex::~moMutex()
{
	// Note that the following reference count test only ensure that
	// you don't delete a mutex which is still locked; however, if
	// you still have multiple threads running, we can't really know
	// if another thread is not just about to use this thread...
	assert(f_reference_count == 0UL);
#ifdef MO_THREAD
	pthread_cond_destroy(&f_cond);
	pthread_mutex_destroy(&f_mutex);
#endif
}



/************************************************************ DOC:

CLASS

	moMutex

NAME

	Lock - lock a mutex
	TryLock - try to lock a mutex
	Unlock - unlock a mutex

SYNOPSIS

	void Lock(void);
	bool TryLock(void);
	void Unlock(void);

DESCRIPTION

	Once you created a mutex, you can use it to synchronize
	multiple threads to use the same resources one at a time.

	Only one thread can Lock() a mutex. Other threads will be
	forced to wait until that one thread finishes with the
	mutex before the other threads continue. It is usually a
	good idea to not lock a mutex for too long. The same thread
	can safely call the Lock() function multiple times. The
	Unlock() then needs to be called the same number of times
	to allow the other threads to start again.

	The TryLock() is similar to lock however it is non-blocking.
	If another thread already owns the mutex, then the function
	returns right away with false.

	For each call to Lock() or a successful TryLock() you need
	to call the Unlock() function. (Our mutexes are always
	re-entrant).

NOTES

	Trying to destroy a mutex which is still in a lock state will
	result in an assert in debug mode (it is likely to crash in
	release since the mutex will be destroyed when someone is
	still going to call Unlock() on it.)

SEE ALSO

	Wait(), TimedWait(), DatedWait(), Signal()

*/
void moMutex::Lock(void)
{
#ifdef MO_THREAD
	pthread_mutex_lock(&f_mutex);
#endif
	// NOTE: we don't need an atomic call since we
	//	 already know we're alone running here...
	++f_reference_count;
}


bool moMutex::TryLock(void)
{
	bool		result;

#ifdef MO_THREAD
	result = pthread_mutex_trylock(&f_mutex) == 0;
#else
	// No threading capability means we can't lock anything
	result = true;
#endif
	if(result) {
		// NOTE: we don't need an atomic call since we
		//	 already know we're alone running here...
		++f_reference_count;
	}

	return result;
}


void moMutex::Unlock(void)
{
	// We can't unlock if it wasn't locked before!
	assert(f_reference_count > 0UL);

	// NOTE: we don't need an atomic call since we
	//	 already know we're alone running here...
	--f_reference_count;

	// When there is no threading capability it means we can't lock
	// and thus there is nothing to unlock
#ifdef MO_THREAD
	pthread_mutex_unlock(&f_mutex);
#endif
}




/************************************************************ DOC:

CLASS

	moMutex

NAME

	Wait - wait on a mutex
	TimedWait - wait on a mutex for specified amount of time
	DatedWait - wait on a mutex until at most specified date
	Signal - signal a mutex

SYNOPSIS

	void Wait(void);
	bool TimedWait(unsigned long long usec);
	bool DatedWait(unsigned long long msec);
	void Signal(void);

DESCRIPTION

	At times it is necessary for one thread to wait on another
	until a certain task is accomplished. For instance, the
	moEventPipe object owner can wait until another thread
	posts an event in the pipe.

	A thread can wait forever (Wait), wait for a certain amount
	of time (TimedWait) or wait until a specified date (DatedWait).

	In all cases, you can't call a wait function without first
	locking the mutex. In debug mode, the mutex will assert if
	it isn't locked. Also, this most certainly means that you
	don't have a clean state to wait on!

	A thread can signal another using the Signal() call. Note
	that a proper signal call needs to be done when the mutex is
	locked, especially if you setup a variable in a certain way
	that another thread could modify too. However, this call will
	make sure the mutex is locked for you. So you don't absolutly
	have to lock it yourself, thought it still is a good idea.

	Note that if a thread signalled a condition, the condition may
	still be true even if the signal happened a while back and no
	thread was waiting on it. This means you can't be sure that
	you will be able to call Wait() and not return immediatly.
	It is the caller responsability to ensure that the Wait() did
	indeed occur as wanted. For instance, in case of the
	moEventPipe, we first check whether the pipe is empty.
	If empty, the Wait() is called and blocks until a new event
	is posted. However, the Wait() may return before the pipe is
	empty in case the mutex was signalled and the Wait() wasn't
	called in between signals being sent. [Note: one can't really
	count on the system to behave one way or the other; this will
	be implementation dependent; in general, Unix thread will
	reset the condition variables when a thread waits on them]

RETURN VALUE

	The TimedWait() and DatedWait() functions return true if a
	signal was received, and false otherwise. Whenever false is
	returned, it can be because the wait timed out or because
	the thread received a signal (see kill(2)).

SEE ALSO

	Lock(), Unlock(), TryLock()

*/
void moMutex::Wait(void)
{
	// For any mutex wait to work, we MUST have the
	// mutex locked already and just one time.
	// NOTE: the 1 time is just for assurance that it will
	// work in most cases; it should even when locked multiple
	// times, but we better make sure!
	assert(f_reference_count == 1UL);
#ifdef MO_THREAD
	pthread_cond_wait(&f_cond, &f_mutex);
#endif
}



bool moMutex::TimedWait(unsigned long long usec)
{
	// For any mutex wait to work, we MUST have the
	// mutex locked already and just one time.
	assert(f_reference_count == 1UL);
#ifdef MO_THREAD
	struct timespec timeout;

#ifdef WIN32
	struct _timeb mstime;

	_ftime(&mstime);
	timeout.tv_sec = static_cast<long>(mstime.time + usec / 1000000ULL);
	usec = mstime.millitm * 1000ULL + usec % 1000000ULL;
#else
	struct timezone ztime;
	struct timeval vtime;

	gettimeofday(&vtime, &ztime);
	timeout.tv_sec = vtime.tv_sec + usec / 1000000ULL;
	usec = vtime.tv_usec + usec % 1000000ULL;
#endif
	if(usec > 1000000ULL) {
		timeout.tv_sec++;
		usec -= 1000000ULL;
	}
	timeout.tv_nsec = static_cast<long>(usec * 1000ULL);
	return pthread_cond_timedwait(&f_cond, &f_mutex, &timeout) == 0;
#else
	// No threading capability means we can't wait
	return false;
#endif
}



bool moMutex::DatedWait(unsigned long long msec)
{
	// For any mutex wait to work, we MUST have the
	// mutex locked already and just one time.
	assert(f_reference_count == 1UL);
#ifdef MO_THREAD
	struct timespec timeout;

	timeout.tv_sec = static_cast<long>(msec / 1000ULL);
	timeout.tv_nsec = static_cast<long>((msec % 1000ULL) * 1000000ULL);
	return pthread_cond_timedwait(&f_cond, &f_mutex, &timeout) == 0;
#else
	// No threading capability means we can't wait
	return false;
#endif
}


void moMutex::Signal(void)
{
#ifdef MO_THREAD
	pthread_mutex_lock(&f_mutex);
	pthread_cond_signal(&f_cond);
	pthread_mutex_unlock(&f_mutex);
#endif
}





/************************************************************ DOC:

CLASS

	moSignal

NAME

	Wait - wait for a signal
	TimedWait - wait for a signal for specified amount of time
	DatedWait - wait for a signal until at most specified date
	Signal - wakeup the thread waiting for a signal

SYNOPSIS

	void Wait(void);
	bool TimedWait(unsigned long long usec);
	bool DatedWait(unsigned long long msec);
	void Signal(void);

DESCRIPTION

	This object hides the necessary lock/unlock of an moMutex
	object in order to call the different Wait() functions.

	This can be used for threads which just need to wait for a
	signal and continue.

RETURN VALUE

	The TimedWait() and DatedWait() functions return true if a
	signal was received, and false otherwise. Whenever false is
	returned, it can be because the wait timed out or because
	the thread received a system signal (see kill(2)).

SEE ALSO

	The corresponding moMutex functions

*/
void moSignal::Wait(void)
{
	Lock();
	moMutex::Wait();
	Unlock();
}

bool moSignal::TimedWait(unsigned long long usec)
{
	Lock();
	bool result = moMutex::TimedWait(usec);
	Unlock();
	return result;
}

bool moSignal::DatedWait(unsigned long long msec)
{
	Lock();
	bool result = moMutex::DatedWait(msec);
	Unlock();
	return result;
}

void moSignal::Signal(void)
{
	moMutex::Signal();
}




};			// namespace molib;

// vim: ts=8

