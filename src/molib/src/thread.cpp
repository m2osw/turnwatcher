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
#pragma implementation "mo/mo_thread.h"
#endif

#include	"mo/mo_thread.h"

namespace molib
{




/************************************************************ DOC:

CLASS

	moThread

NAME

	Constructor - initialize a thread object
	Destructor - suspends the thread if still running and cleans up

SYNOPSIS

	moThread(const moName& name, const moRunner *runner);
	virtual ~moThread();
	static bool ThreadingAvailable(void);

PARAMETERS

	name - the name of this thread
	runner - the task to run by this thread

DESCRIPTION

	The moThread and moThread::moRunner classes are used by the
	mo library users to create applications using multiple threads.

	The constructor initializes the moThread object but it doesn't
	start the thread. You need to call Start() in order to get
	the thread running. The code run by the thread will be the
	Run() function of the specified runner. The runner cannot be
	changed for the lifetime of the moThread object.

	The name passed on the moThread object is used to broadcast
	the death event of the thread. It is thus important to properly
	choose this name.

	The pointer to the runner can be set to null in which case the
	new thread is considered empty and the Start() function will
	always fail.

	The destructor calls the Stop() function which requests the
	moRunner to stop. Note that we have a separate object because
	when the destructor of the moThread is called, the moThread
	object is in an invalid state (virtual tables will have been
	changed to match the moThread virtual functions) and thus it
	cannot be the object including the code for the thread. This
	means you cannot derive your class from moThread and moRunner
	(it is actually strongly suggested that you never derive from
	moThread.)

	The ThreadingAvailable() function can be used to know whether
	threads are available in the running application. It returns
	true if threads can be created (this doesn't guarantee that
	threads will successfully be created though.)

	This is a static function and thus you don't have to create
	an moThread object to call it, i.e.:

		if(!moThread::ThreadingAvailable()) {
		    fprintf(stderr, "ERROR: my program requires threads\n");
		    exit(1);
		}

	To implement a thread correctly you should do the following in
	your class derived from moThread::moRunner:

	. Override the Init() function; do initialization you need to do
	  for the new thread to function properly; if your initialization
	  succeeds, return true; if you want the parent thread to wait
	  for this initialization to be complete then also implement
	  the IsWaitStartedRequired() and return true

	. Override the Run() function; DO NOT IMPLEMENT A LOOP! The
	  loop is outside and you may not use it by leaving the run
	  count to the default value (i.e. run once); if Run() can
	  fail, return false, it will stop the thread; otherwise
	  always return true

	. Override the Cleanup() function; do all the necessary cleanup
	  of resources allocated by the thread.

	Example:

		bool Init()
		{
			m_state = 0;

			return true;
		}

		bool Run()
		{
			switch(m_state) {
			case 0:
				// do this
				m_state = 1;
				break;

			case 1:
				// do that
				m_state = 2;
				break;

			case 2:
				// do more and then repeat
				m_state = 0;
				break;

			}

			return true;
		}

		void Cleanup()
		{
			if(m_state == 1) {
				// do some necessary cleanup
			}
		}

BUGS

	The runner objects can be owned by at most one moThread at a
	time. However they can be handled by some temporary moThread
	and then another and another... This rule is being enforced
	at run time by a throw in the constructor whenever an moRunner
	is being attached to a second moThread.

RETURN VALUE

	The ThreadingAvailable() function returns true if threads
	capability is available, false otherwise.

SEE ALSO

	GetName(), IsRunning(), Start(), Stop(), CallDeathEventPipes()

*/
moThread::moThread(const moName& name, const moRunner *runner)
	: f_name(name),
	  f_running(false),
	  f_wait_ended(false),
	  f_is_wait_started_required(runner == 0 ? false : runner->IsWaitStartedRequired()),
	  f_run_count(RUN_COUNT_STOP),
	  f_runner(runner)
{
	//f_mutex -- auto-init
	//f_death_mutex -- auto-init
	//f_started_signal -- auto-init
	//f_ended_signal -- auto-init
	//f_death_event_pipes -- auto-init

	if(f_runner) {
		if(!f_runner->SetThread(this)) {
			throw moError(MO_ERROR_IN_USE, "your moRunner object is already attached to an moThread");
		}
	}

#ifdef MO_THREAD
	// The following doesn't compile on Macs and it doesn't seem to
	// be necessary since we have f_running to know whether it is
	// valid
	//f_thread = static_cast<pthread_t>(-1);	// invalid thread number (just a guess...)

	pthread_attr_init(&f_thread_attr);

	// avoid having to call join() on threads
	// we handle our own signals as required
	pthread_attr_setdetachstate(&f_thread_attr, PTHREAD_CREATE_DETACHED);
#endif
}


moThread::~moThread()
{
	// make sure the runner is stopped before
	// to completely destroy a thread object
	Stop(true);
	if(f_runner) {
		// detach ourself from the runner
		f_runner->SetThread(0);
	}
#ifdef MO_THREAD
	pthread_attr_destroy(&f_thread_attr);
#endif
}


const char *moThread::moGetClassName(void) const
{
	return "molib::moThread";
}


bool moThread::ThreadingAvailable(void)
{
#ifdef MO_THREAD
	return true;
#else
	return false;
#endif
}




/************************************************************ DOC:

CLASS

	moThread

NAME

	GetName - get the name of this thread

SYNOPSIS

	mo_name_t GetName(void) const;

DESCRIPTION

	Each moThread object needs to be given a name when created.

	This name is constant and can be read with the GetName()
	function.

RETURN VALUE

	the name of the thread as defined by the moNamePool

SEE ALSO

	Constructor

*/
mo_name_t moThread::GetName(void) const
{
	return f_name;
}


/************************************************************ DOC:

CLASS

	moThread

NAME

	IsRunning - check whether this thread is currently running
	Start - start the thread
	Stop - stop the thread and eventually wait for it to be stopped
	Continue - whether the Stop() function was called

SYNOPSIS

	virtual bool IsRunning(void) const;
	bool Start(run_count_t run_count = RUN_COUNT_DEFAULT);
	void Stop(bool wait = false);
	bool Continue(void) const;

PARAMETERS

	run_count - the number of time the Run() function will be called
	wait - whether the Stop() command waits for the thread to end

DESCRIPTION

	Once a thread was initialized, you can start it (in effect,
	call its Init()/Run()/Cleanup() functions) by calling the
	Start() function.

	The Start() command works if:

	. run_count is not RUN_COUNT_STOP

	. a valid runner was attached to the moThread (i.e. not null)

	. the thread is not already running

	. the runner is ready

	. the system can create one more thread

	Once started, the system thread runs until one of the following
	events occur:

	. the Init() function returns false (in which case the Run()
	  function will not even be called)

	. the run_count parameter reaches RUN_COUNT_STOP (zero)

	. the Run() function returns false

	. the Stop() function is called, forcing the run_count counter
	  to the value RUN_COUNT_STOP (within the thread, you can test
	  this value with a call to Continue())

	When the thread dies, the following happens:

	. call the Cleanup() function

	. set the IsRunning() flag to false

	. post a message to all the death event handlers

	. signal the thread which called Stop(), if any, with the wait
	  flag set to true

	You can thus stop a thread by calling its Stop() function.
	Note that the Stop() function is likely to return before
	the thread dies unless you set the wait flag to true. A
	thread can call its own Stop() function in which case it
	will not block (i.e. ignore the wait flag.)

	The IsRunning() function returns true if the thread is
	currently running. Note however that the running flag is set
	to false before the death events are posted. This gives a
	chance to other threads to try restarting this thread before
	it really exits. This will block the new parent thread until
	the child is finished posting events.

	The Continue() function returns true until the Stop() function
	is called or the run_count counter reaches zero
	(i.e. RUN_COUNT_STOP). This function is usually called from
	the Continue() defined in the moRunner class.

NOTES

	All the functions are supposed to be thread safe.

BUGS

	Multiple threads can call the Stop() function with the
	'wait' parameter set to 'false'.

	When the parameter 'wait' is to be set to 'true' you need
	to make sure only one single thread calls the Stop()
	function. This is because only one thread can be waiting
	for the other until it dies. If you need to know when the
	thread dies, use the death event facility instead (see the
	AddDeathEventPipe() function).

RETURN VALUE

	IsRunning() returns true when the thread is currently working

	Start() returns false if a new system thread cannot be created
	either because of a lack of resources or because there is
	already a system thread attached to this moThread object or
	you set the run_count parameter to RUN_COUNT_STOP or the
	runner is not ready

	Continue() returns false until the thread starts; then it
	returns true until the Stop() function is called or the run_count
	counter reaches zero; this flag is used to break the thread loop

SEE ALSO

	CallDeathEventPipes(), AddDeathEventPipe()

*/
bool moThread::IsRunning(void) const
{
	return f_running;
}


#ifdef MO_THREAD
void *__internal_start(void *thread)
{
	moThread *t = reinterpret_cast<moThread *>(thread);
	t->InternalRun();
	return 0;
}


void moThread::InternalRun(void)
{
	if(f_runner->Init()) {
		if(f_is_wait_started_required) {
			f_started_signal.Signal();
		}
		while(f_run_count != RUN_COUNT_STOP) {
			if(!f_runner->Run()) {
				break;
			}
			if(f_run_count != RUN_COUNT_FOREVER) {
				--f_run_count;
			}
		}
	}
	f_runner->Cleanup();
	moLockMutex lock(f_death_mutex);
	f_running = false;
	PostToDeathEventPipes(f_runner->GetResult());

	// signal our death if someone is waiting on it
	if(f_wait_ended) {
		f_ended_signal.Signal();
	}
}
#endif


bool moThread::Start(run_count_t run_count)
{
#ifdef MO_THREAD
	// do we need to even start?
	if(run_count == RUN_COUNT_STOP || !f_runner) {
		return false;
	}

	moLockMutex lock(f_death_mutex);

	if(f_running || !f_runner->IsReady()) {
		// it is already running, we cannot start it more than once
		// or the runner is somehow not ready
		return false;
	}

	f_runner->SetResult(0);		// reset the result on each run

	f_running = true;
	f_run_count = run_count;
	f_wait_ended = false;
	if(f_is_wait_started_required) {
		f_started_signal.Lock();
	}
	int r = pthread_create(&f_thread, &f_thread_attr, &__internal_start, this);
	lock.Unlock();
	if(r != 0) {
		// startup failed
		f_running = false;
	}
	else if(f_is_wait_started_required) {
		f_started_signal.Wait();
	}
	if(f_is_wait_started_required) {
		f_started_signal.Unlock();
	}
	return r == 0;
#else
	// No threading capability means we cannot start
	return false;
#endif
}



void moThread::Stop(bool wait)
{
#ifdef MO_THREAD
	// If there is no thread capability, f_running is always false
	moLockMutex lock(f_death_mutex);

	if(f_running && f_run_count != RUN_COUNT_STOP) {
		f_run_count = RUN_COUNT_STOP;
		pthread_t self = pthread_self();
		if(wait && !pthread_equal(f_thread, self)) {
			f_wait_ended = true;
			lock.Unlock();

fprintf(stderr, "WARNING: waiting for the death of thread %d in Stop()\n", (int) f_name);
			f_ended_signal.Wait();
		}
	}
#endif
}


bool moThread::Continue(void) const
{
	return f_run_count != RUN_COUNT_STOP;
}






/************************************************************ DOC:

CLASS

	moThread

NAME

	AddDeathEventPipe - event pipe to receive death events
	RemoveDeathEventPipe - remove a death event pipe from a thread
	PostToDeathEventPipes - send a death event to all registered pipes

SYNOPSIS

	void AddDeathEventPipe(const moEventPipe& event_pipe);
	void RemoveDeathEventPipe(const moEventPipe& event_pipe);
	void PostToDeathEventPipes(moBase *result) const;

PARAMETERS

	event_handler - an event pipe to sent death events

DESCRIPTION

	The AddDeathEventPipe() function is used to add an
	event pipe to which the death of a thread is to be
	reported. You can add the same handler multiple times
	if you want. It will work, thought, it probably isn't
	very useful since it will duplicate the message sent
	to your pipe.

	Note that the death is reported by the thread right before
	it dies and thus it may still be running, system speaking,
	whenever this event arrives to your handler.

	If after a while you are not interested in receiving
	death events, you can use the RemoveDeathEventPipe()
	function to remove your event pipe. Note that if you
	added the same handler multiple times, you will only
	remove one instance per call to RemoveDeathEventPipe().

	The PostToDeathEventPipes() is currently public. It may
	become private or protected. At this time, it isn't known
	whether anyone will even need to call that function
	directly, but by doing so you can simulate the death of
	a thread.

	The result parameter passed to PostToDeathEventPipes()
	is the result set in your moRunner before it returns.
	Do not forget that this result is cleared (set to null)
	each time you restart the thread.

NOTES

	These functions are protected with a mutex when handling
	the list of pipes. Thus it is safe to add and remove
	pipes even when a handler is called. Note however that
	removing a handler which has not yet been called will
	not prevent that handler from being called if it was
	in the list before the call to the PostToDeathEventPipes()
	function.

SEE ALSO

	IsRunning(), Start(), Stop()

*/
void moThread::AddDeathEventPipe(const moEventPipe& event_pipe)
{
	moLockMutex lock(f_mutex);

	f_death_event_pipes += event_pipe;
}


void moThread::RemoveDeathEventPipe(const moEventPipe& event_pipe)
{
	moList::position_t	idx;

	moLockMutex lock(f_mutex);

	idx = f_death_event_pipes.Count();
	while(idx > 0UL) {
		idx--;
		if(f_death_event_pipes.Get(idx) == &event_pipe) {
			f_death_event_pipes.Delete(idx);
			return;
		}
	}
}


void moThread::PostToDeathEventPipes(moBase *result) const
{
	moList::position_t	idx;
	moBaseSPtr		data = f_runner->GetData();
	moEventDeath		event(f_name, data, result, f_runner, this);

	// note that this is a set of posts and thus there will be
	// no function being called which could in turn call the
	// add or remove functions messing up the list of death
	// events and cause crashes
	moLockMutex lock(f_mutex);

	idx = f_death_event_pipes.Count();
	while(idx > 0UL) {
		idx--;
		f_death_event_pipes.Get(idx)->Post(event);
	}
}





/************************************************************ DOC:

CLASS

	moThread::moRunner

NAME

	Constructor - initialize the runner
	Destructor - clean up the runner
	moGetClassName - returns the name of the moRunner class

SYNOPSIS

	moThread(void);
	~moThread();
	moGetClassName(void);

DESCRIPTION

	The constructor ensures that the default f_thread pointer
	is null.

	The destructor does nothing in release. In debug it checks
	that the f_thread pointer is null. If not it is a bug since
	the runner should not be deleted if still attached to an
	moThread object.

	The moGetClassName() function returns the name of the class.

SEE ALSO

	moThread

*/
moThread::moRunner::moRunner(void)
	: f_thread(0)
{
}


moThread::moRunner::~moRunner()
{
	assert(f_thread == 0);
}


const char *moThread::moRunner::moGetClassName(void) const
{
	return "molib::moThread::moRunner";
}



/************************************************************ DOC:

CLASS

	moThread::moRunner

NAME

	GetThread - returns a smart pointer to the thread
			attached to this runner

	private:
	SetThread - attach a thread to this runner

SYNOPSIS

	moThreadSPtr GetThread(void) const;
	bool SetThread(const moThread *thread);

DESCRIPTION

	The GetThread() function returns a smart pointer of the
	thread attached to this runner. You must save that thread
	in a smart pointer as well. It will return a null pointer
	whenever the runner is not attached.

	SetThread() is a private function called by the moThread
	to set itself as the owner of the runner. Note that there
	can be only one owner. Trying to attach more than one thread
	to a runner fails with a throw of an moError in the moThread
	constructor.

SEE ALSO

	moThread

*/
moThreadSPtr moThread::moRunner::GetThread(void) const
{
	// the return may look atomic, but the initialization
	// of the smart pointer is not
	moLockMutex lock(f_mutex);
	return f_thread;
}


bool moThread::moRunner::SetThread(const moThread *thread)
{
	// here we're saving a bare pointer since we
	// do not want to have a looping reference
	// which would prevent the moThread from being
	// destroyed as expected by the end users
	moLockMutex lock(f_mutex);

	// already attached?
	if(f_thread != 0 && thread != 0) {
		return false;
	}

	f_thread = const_cast<moThread *>(thread);

	return true;
}



/************************************************************ DOC:

CLASS

	moThread::moRunner

NAME

	IsWaitStartedRequired - check whether this thread
		requires the parent to wait until the child
		initialization is complete
	IsReady - check whether the runner is ready to start
	Init - initialize the new thread
	Run - run the new thread once (see below)
	Cleanup - cleans the thread before exiting
	Continue - test whether the thread should stop

SYNOPSIS

	virtual bool IsWaitStartedRequired(void) const;
	virtual bool IsReady(void) const;
	virtual bool Init(void);
	virtual bool Run(void);
	virtual void Cleanup(void);
	bool Continue(void) const;

DESCRIPTION

	The IsWaitStartedRequired() function is used to know whether
	the Start() function needs to wait on the Init() function to
	complete before to continue. The IsWaitStartedRequired() function
	is called only once when the runner is passed to the constructor.
	This is important to ensure consistency. Yet, since this should
	always return true when overridden, it should not be a problem.

	The IsReady() function is called from the parent thread before
	the child is created to make sure that the moRunner is ready
	to start. If that call returns false, then the child is not
	started and the Start() function returns false. The default
	function always returns true.

	The Init() function is called once when the thread starts.
	It can be used to initialize the new thread. When your
	IsWaitStartedRequired() function returned true in the
	moThread constructor, the parent thread blocks until this
	Init() function returns.

	The default Init() function just returns true.

	The Run() function is called repeatitively by the new thread
	until the thread is stopped. You can stop the thread by:
	
	a) returning false from the Run() function;
	b) calling the Stop() function from any thread;
	c) reaching the maximum number of times the Run() function
	   is supposed to be called (number which is specified with
	   the Start() function.)

	You can prevent (a) by always returning true from the Run()
	function. You can prevent (c) by calling the Start() function
	with RUN_COUNT_FOREVER. You can prevent (a), (b) and (c) by
	never returning from the Run() function.

	If your Run() function is very long, you may want to break
	it up in smaller tasks which you perform one after another.
	You can also call the Continue() function within the Run()
	function to know whether another thread requested you to
	Stop() [i.e. if(!Continue()) return false;]

	The Cleanup() function is called once the thread was
	asked to stop. This function is expected to release any
	resource used by the thread.

	The default Cleanup() function does nothing.

	After the Cleanup() the child thread will post its death
	event. Finally, if Stop() was called by another thread and
	that thread requested to wait on the child death (i.e. a
	blocking Stop() call), the child signals the waiting thread.

	The Continue() function can be called from within your
	functions in the running thread to know whether the user
	requested the thread to stop (i.e. called the Stop()
	function.) This can be useful if you have a very long
	process which can cleanly be stopped mid-way.

	Note that the Continue() function is public so other threads
	can check whether this thread is stopped or about to stop.
	Yet, to make sure of that, you should call the
	corresponding moThread::IsRunning() function instead.

SEE ALSO

	IsRunning(), Start(), Stop()

*/
bool moThread::moRunner::IsWaitStartedRequired(void) const
{
	return false;
}


bool moThread::moRunner::IsReady(void) const
{
	return true;
}


bool moThread::moRunner::Init(void)
{
	return true;
}


void moThread::moRunner::Cleanup(void)
{
}


bool moThread::moRunner::Continue(void) const
{
	moLockMutex lock(f_mutex);

	// should always be true unless called from outside
	if(f_thread != 0) {
		return f_thread->Continue();
	}

	return false;
}







/************************************************************ DOC:

CLASS

	moThread::moRunner

NAME

	GetData - retrieve the pointer to the data passed to the thread
	SetData - set the data to be used by the thread
	GetResult - the current result of the thread
	SetResult - set a new result for the thread

SYNOPSIS

	moBaseSPtr GetData(void) const;
	void SetData(moBase *data);
	moBaseSPtr GetResult(void) const;
	void SetResult(moBase *result);

PARAMETERS

	data - a pointer to some moBase object
	result - a pointer to some moBase object

DESCRIPTION

	These functions manage the data (input) and result (output) of
	a thread. The functions are mutually thread safe meaning that
	only one thread at a time can change or read the data or
	result pointer.

	Note that the GetData() and GetResult() functions return a
	smart pointer. This means the object will not be deleted
	between a call to that function and the time the caller
	uses the pointer. It is your responsibility to save that
	pointer in a smart pointer.

BUGS

	These functions by themselves are safe, however changing the
	data pointer or the result objects may not be thread safe.
	Refer to that object for more information.

SEE ALSO

	IsRunning(), Start(), Stop()

*/
moBaseSPtr moThread::moRunner::GetData(void) const
{
	moLockMutex lock(f_mutex);
	return f_data;
}


void moThread::moRunner::SetData(moBase *data)
{
	moLockMutex lock(f_mutex);
	f_data = data;
}


moBaseSPtr moThread::moRunner::GetResult(void) const
{
	moLockMutex lock(f_mutex);
	return f_result;
}


void moThread::moRunner::SetResult(moBase *result)
{
	moLockMutex lock(f_mutex);
	f_result = result;
}










/************************************************************ DOC:

CLASS

	moTaskManager::moTask

NAME

	Constructor - initialize a task object
	Destructor - uninitialize a task object

SYNOPSIS


DESCRIPTION

	The moTask object will be used in an moTaskManager object
	in order to run tasks in a seperate thread.

	The idea behind having a task is to do some work
	asynchroneously without overloading the processor. Thus,
	instead of having one thread per task, you have many tasks
	which will be run in a single thread. Once a task if finished,
	you need to kill it. It will then automatically be removed
	from the task manager.

	The moTaskManager will call the Run() function of every task
	it holds, one after another. It then starts again. The Run()
	function is expected to return after a relatively small amount
	of time.

	A task can either be repetitive, so each time the Run() function
	is called it does the same thing over and over again, or it
	can be written using a state. Thus, it can do some specialized
	work each time the Run() function is called.

SEE ALSO

	Run(), SetState(), SetOrder(), SetPriority()

*/
moTaskManager::moTask::moTask(const moName& name)
	: f_state(MO_TASK_STATE_STARTING),
	  f_name(name),
	  f_attached(0)		// unfortunately, atomic words can't be auto initialized
{
}


moTaskManager::moTask::~moTask()
{
	// we can't delete a task which is attached
	// (this shouldn't happen since the moTaskManager's
	// will AddRef() tasks)
	assert(f_attached == 0);
}

const char *moTaskManager::moTask::moGetClassName(void) const
{
	return "molib::moTask";
}



/************************************************************ DOC:

CLASS

	moTaskManager::moTask

NAME

	SetState - change the current state of a task
	GetState - get the current state of a task

SYNOPSIS

	void SetState(state_t state);
	state_t GetState(void) const;

PARAMETERS

	state - the new state

DESCRIPTION

	An moTask object can be in one of the following states:

		MO_TASK_STATE_STARTING
		MO_TASK_STATE_RUNNING
		MO_TASK_STATE_SLEEPING
		MO_TASK_STATE_ZOMBIE

	An moTask is marked as STARTING until it is added to
	an moTaskManager, at which time its state becomes
	RUNNING meaning its Run() function will be called
	when allowed.

	You can temporarilly stop a task by setting its state
	to SLEEPING. Another task or thread can later revive
	the task by changing its state back to RUNNING. Note
	that setting the state to SLEEPING doesn't put a
	task to sleep if it is currently running. It will
	really be asleep once its Run() function returns.
	A task can put itself in SLEEPING mode.

	At any time, you can set the state to ZOMBIE to kill
	a task. If you try to add a ZOMBIE task to an moTaskManager,
	then nothing happens. When a task is not part of a
	task manager, then it's state can be set to STARTING
	again.

NOTES

	The fact that a task can't be changed from ZOMBIE to
	RUNNING or SLEEPING seems to make sense to me at this
	time. You can detach the task from the task manager
	and then reattach it after you changed the state
	back to STARTING. This ensures you know what you're
	doing.

RETURN VALUE

	The GetState() function returns the current state of
	a task.

SEE ALSO

	Run(), GetOrder(), GetPriority()

*/
void moTaskManager::moTask::SetState(state_t state)
{
	if(f_attached == 0) {
		if(state == MO_TASK_STATE_STARTING
		|| state == MO_TASK_STATE_ZOMBIE) {
			f_state = state;
		}
		return;
	}

	// when attached, we can't change the state to
	// STARTING; we first need to get detached and
	// then reattached if necessary
	if(state == MO_TASK_STATE_STARTING) {
		return;
	}

	// we can't get a zombie out of the tomb as long
	// as the task is attached
	if(f_state == MO_TASK_STATE_ZOMBIE) {
		return;
	}

	f_state = state;
}


moTaskManager::moTask::state_t moTaskManager::moTask::GetState(void) const
{
	return f_state;
}



/************************************************************ DOC:

CLASS

	moTaskManager::moTask

NAME

	SetOrder - change the order of this task
	GetOrder - get the order of this task
	SetPriority - change the priority of this task
	GetPriority - get the priority of this task
	NeedSorting - whether the priority or order changed
	Compare - compare two tasks together for sorting purposes

SYNOPSIS

	void SetOrder(int order);
	int GetOrder(void) const;
	void SetPriority(int priority);
	int GetPriority(void) const;

	bool NeedSorting(void) const;

	virtual compare_t Compare(const moBase& object) const;

PARAMETERS

	order - the new order
	priority - the new priority
	object - another moTask object

DESCRIPTION

	Tasks are sorted first by priority and second by
	order. The priority and order can be changed at
	any time, but only when a task returns does it
	have an effect on the list of tasks.

	The tasks with a higher priority (i.e. 10 is higher
	than 5) will always be executed until they fall asleep.
	If they don't fall asleep, then tasks with a lower
	priority won't get any CPU time for themselves.

	The order can be used to execute certain tasks
	before others. Contrary to the priority, the lower
	order is executed first. Thus, a task with an order
	of 10 will be executed after a task with an order
	of -10.

	By default, the order and priority of a task
	are set to 0.

BUGS

	At this time this is somewhat bogus since it is
	not multithreaded aware. However, it should still
	work in most cases. The main problem is that we
	would need a global mutex to make sure everything
	is properly synchronized. Thus the sorting would
	be done while the mutex is locked by a task
	manager.

RETURN VALUE

	The GetOrder() function returns the order value.

	The GetPriority() function returns the priority value.

SEE ALSO

	Run(), GetState()

*/
void moTaskManager::moTask::SetOrder(int order)
{
	if(f_order != order) {
		f_order = order;
		f_need_sorting = true;
	}
}


int moTaskManager::moTask::GetOrder(void) const
{
	return f_order;
}


void moTaskManager::moTask::SetPriority(int priority)
{
	if(f_priority != priority) {
		f_priority = priority;
		f_need_sorting = true;
	}
}


int moTaskManager::moTask::GetPriority(void) const
{
	return f_priority;
}


bool moTaskManager::moTask::NeedSorting(void) const
{
	return f_need_sorting;
}


moBase::compare_t moTaskManager::moTask::Compare(const moBase& object) const
{
	const moTask& task = dynamic_cast<const moTask&>(object);

	// WARNING: we are sorting in "reverse" order, that is, the
	//	    largest priority first;
	compare_t r = CompareInt(task.f_priority, f_priority);
	if(r != MO_BASE_COMPARE_EQUAL) {
		return r;
	}

	// NOTE: order is compared the usual way (not "reversed")
	return CompareInt(f_order, task.f_order);
}



/************************************************************ DOC:

CLASS

	moTaskManager::moTask

NAME

	GetName - get the name of this task

SYNOPSIS

	mo_name_t GetName(void) const;

DESCRIPTION

	Get the name of the task. Any task is required to have
	a name.

RETURN VALUE

	The name of the task as defined in an moNamePool.

SEE ALSO

	Run(), Constructor

*/
mo_name_t moTaskManager::moTask::GetName(void) const
{
	return f_name;
}


/************************************************************ DOC:

CLASS

	moTaskManager::moTask

NAME

	Attach - attach this task to a task manager
	Detach - detach this task from a task manager

SYNOPSIS

	void Attach(void);
	void Detach(void);

DESCRIPTION

	Each time a task is added to a task manager, that task
	manager calls the Attach() function. Each time a task gets
	in a Zombie state, the Detach() function is called. This
	ensures we know whether the task is currently part of a
	task manager or not.

	Note that a task can be added to different task managers.
	It is the responsability of the creator of such task
	to make sure they will run properly in multiple threads
	in parallel.

SEE ALSO

	Run(), Constructor

*/
void moTaskManager::moTask::Attach(void)
{
	moAtomicAdd(&f_attached, 1);
}


void moTaskManager::moTask::Detach(void)
{
	moAtomicAdd(&f_attached, -1);
}


/************************************************************ DOC:

CLASS

	moTaskManager

NAME

	Constructor - initialize a task manager
	Destructor - cleans up a task manager
	moGetClassName - retrieve the name of the class

SYNOPSIS

	moTaskManager(void);
	~moTaskManager();
	virtual const char *moGetClassName(void) const;

DESCRIPTION

	One will create a task manager whenever multiple tasks need
	to run in the background one after another (opposed to
	separate threads which would run in parallel.)

SEE ALSO

	Run(), Constructor

*/
moTaskManager::moTaskManager(const moName& name)
	: f_idx_task(0)
{
}

moTaskManager::~moTaskManager()
{
}


const char *moTaskManager::moGetClassName(void) const
{
	return "molib::moTaskManager";
}



/************************************************************ DOC:

CLASS

	moTaskManager

NAME

	IsRunning - check whether the task manager is currently running

SYNOPSIS

	virtual bool IsRunning(void) const;

	private:
	virtual bool Init(void);
	virtual bool Run(void);
	virtual void Cleanup(void);

DESCRIPTION

	The task manager thread can be running continuously. However,
	it can be waiting for tasks to be assigned to it or have only
	sleeping tasks and in that case it will be considered as not
	running.

	The IsRunning() function returns true if the moTaskManager
	thread was started and some of the tasks assigned to it are
	running. Note that if you need to know whether the corresponding
	thread is running, then call IsRunning() on the thread and not
	the task manager.

	The Run() function is that one function which will
	handle the list of tasks to know which one needs to be run next
	and which ones need to be removed from the list of tasks. This
	is an internal function.

	The Init() function initializes the task index. It will be
	re-initialized each time you restart the Task Manager thread.

	The Cleanup() function ensures that the internal lock isn't
	in effect when the thread ends.

BUGS

	To stop the Task Manager thread you can call Stop() this will
	stop all the tasks. If there was no task or all tasks were
	sleeping then the Stop() may need up to 2 seconds to really
	stop the Task Manager thread.

RETURN VALUE

	IsRunning() returns true when the task manager is
	considered running

SEE ALSO

	Start(), AddTask()

*/
bool moTaskManager::IsRunning(void) const
{
	moTask::state_t		state;
	moList::position_t	idx;
	moTaskSPtr		task;
	moThreadSPtr		thread;

	// are we running at all?
	thread = GetThread();
	if(!thread || !thread->IsRunning()) {
		return false;
	}

	moLockMutex lock(f_mutex);

	idx = f_tasks.Count();
	while(idx > 0UL) {
		--idx;
		task = f_tasks.Get(idx);
		state = task->GetState();
		if(state == moTask::MO_TASK_STATE_RUNNING) {
			return true;
		}
	}

	return false;
}


bool moTaskManager::Init(void)
{
	f_idx_task = 0;
	f_mutex.Lock();

	return true;
}


bool moTaskManager::Run(void)
{
	moTaskSPtr		task, next;
	moList::position_t	idx, max, count;
	moTask::state_t		state;
	bool			need_sorting;

	// First, we check whether we need to re-order the tasks
	need_sorting = false;
	idx = f_tasks.Count();
	while(idx > 0UL) {
		--idx;
		task = f_tasks.Get(idx);
		state = task->GetState();
		if(state == moTask::MO_TASK_STATE_ZOMBIE) {
			// Get rid of zombies as we're at it
			f_tasks.Delete(idx);
			task = 0;
			continue;
		}
		if(task->NeedSorting()) {
			need_sorting = true;
		}
	}

	if(need_sorting) {
		// the following two lines will re-sort the f_tasks list
		moList list(f_tasks);
		f_tasks = list;
	}

	// work on one task and exit the loop;
	max = f_tasks.Count();
	for(count = 0; count < max; ++count) {
		if(f_idx_task >= max) {
			f_idx_task = 0;
		}
		task = f_tasks.Get(f_idx_task);
		++f_idx_task;
		state = task->GetState();
		if(state == moTask::MO_TASK_STATE_RUNNING) {
			// The priority test is not so good in
			// case f_tasks.Count() changes; we may
			// want to revisit this code at a later
			// time... this is the very easy way to
			// do it!
			if(f_idx_task < max) {
				next = f_tasks.Get(f_idx_task);
				if(next->GetPriority() < task->GetPriority()) {
					f_idx_task = 0;
				}
			}
			f_mutex.Unlock();
			// WARNING: this will work because the
			//	    task pointer is an moSmartPtr;
			//	    this means the task has one
			//	    AddRef() when running; this is
			//	    important since the RemoveTask()
			//	    could otherwise delete the
			//	    task before it has returned!
			task->Run(this);
			// for debug purposes, we could add this
			//task = 0;
			f_mutex.Lock();
			break;
		}
	}

	// if all tasks are sleeping or there isn't any task
	// then we want to wait for someone to send us a task
	if(count == max) {
		// wait for 1 sec. then return because the user
		// may want to stop our thread...
		f_mutex.TimedWait(1000000);
	}

	return true;
}


void moTaskManager::Cleanup(void)
{
	f_mutex.Unlock();
}




/************************************************************ DOC:

CLASS

	moTaskManager

NAME

	AddTask - add a task to the task manager
	RemoveTask - remove a task from the task manager

SYNOPSIS

	void AddTask(moTask& task);
	void RemoveTask(moTask& task);

DESCRIPTION

	Once you created a task manager, you can add and remove
	tasks from it. A task is added with AddTask() and
	removed with RemoveTask(). The same task will be passed
	to either function.

	Note that the RemoveTask() function is relatively slow and
	is not usually necessary. That is, if you set the state of
	a task to MO_TASK_STATE_ZOMBIE, then the task will
	automatically be removed from the list of tasks of the
	manager as soon as possible.

	A task you add can't be in the MO_TASK_STATE_ZOMBIE state
	or it will be ignored. It can however, already be running
	or asleeping as handled by another task manager.

NOTES

	At this time, the fact that a task could be shared between
	multiple task managers is not well supported. It will have
	to be heavily debugged before to be used and this note can
	then be scrapped out.

BUGS

	Note that you can currently add the same task multiple
	times. This feature is likely to be removed in the
	future.

	The same task can be defined in multiple task managers,
	however, a task has only one state shared among all of
	the task managers. This means the same task will fall
	asleep in all task managers or be running in all task
	managers. Similarly, if its state becomes ZOMBIE, then
	it will die and be removed from the all task managers.

RETURN VALUE

	true when the task manager is considered running

SEE ALSO

	Start(), AddTask()

*/
void moTaskManager::AddTask(moTask& task)
{
	// adding a task which is already a zombie has no effect
	moTask::state_t state = task.GetState();
	if(state == moTask::MO_TASK_STATE_ZOMBIE) {
		return;
	}

	// starting the task right away
	if(state == moTask::MO_TASK_STATE_STARTING) {
		task.SetState(moTask::MO_TASK_STATE_RUNNING);
	}

	task.Attach();

	moLockMutex lock(f_mutex);

	f_tasks += task;

	f_mutex.Signal();
}


void moTaskManager::RemoveTask(moTask& task)
{
	moList::position_t	idx;

	// we can't mess up the list without a lock in place
	moLockMutex lock(f_mutex);

	idx = f_tasks.Count();
	while(idx > 0UL) {
		--idx;
		if(&task == f_tasks.Get(idx)) {
			task.SetState(moTask::MO_TASK_STATE_ZOMBIE);
			task.Detach();
			f_tasks.Delete(idx);
		}
	}

	// if the task manager is waiting, then this will wake it up
	// and then it will delete the zombie
	f_mutex.Signal();
}





};			// namespace molib;

// vim: ts=8

