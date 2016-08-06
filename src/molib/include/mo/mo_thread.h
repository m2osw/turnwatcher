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



#ifndef MO_THREAD_H
#define	MO_THREAD_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_EVENT_H
#include	"mo_event.h"
#endif



namespace molib
{



// The moThread class describes how to create a thread within the libmo
// If you want to use a thread to run a task without having to create
// a new thread each time, then use the moTaskManager and moTask classes
class MO_DLL_EXPORT moThread;

typedef moSmartPtr<moThread>	moThreadSPtr;

class MO_DLL_EXPORT moThread : public moBase
{
public:
	class MO_DLL_EXPORT moRunner : public moBase
	{
	public:
					moRunner(void);
		virtual			~moRunner();

		virtual const char *    moGetClassName(void) const;

		// Please, read the docs for more info about these functions
		virtual bool		IsWaitStartedRequired(void) const;
		virtual bool		IsReady(void) const;
		virtual bool		Init(void);
		virtual bool		Run(void) = 0;
		virtual void		Cleanup(void);

		moBaseSPtr		GetData(void) const;
		void			SetData(moBase *data);
		moBaseSPtr		GetResult(void) const;
		void			SetResult(moBase *data);

		moThreadSPtr		GetThread(void) const;

		bool			Continue(void) const;

	private:
		// internal functions
		friend class moThread;
		bool			SetThread(const moThread *thread);

		mutable moMutex		f_mutex;
		moBaseSPtr		f_data;
		moBaseSPtr		f_result;
		moThread *		f_thread;       // weak pointer to avoid circular references
	};

	typedef moSmartPtr<moRunner>	moRunnerSPtr;

	class moEventDeath : public moEvent
	{
	public:
					moEventDeath(const moName& name, moBase *data, moBase *result, const moRunner *runner, const moThread *thread)
						: moEvent("DEATH"),
						  f_name(name),
						  f_data(data),
						  f_result(result),
						  f_runner(runner),
						  f_thread(thread)
					{
					}

		mo_name_t		GetThreadName(void) const { return f_name; }
		moBaseSPtr		GetThreadData(void) const { return f_data; }
		moBaseSPtr		GetThreadResult(void) const { return f_result; }
		moRunnerSPtr		GetThreadRunner(void) const { return f_runner; }
		moThreadSPtr		GetThread(void) const { return f_thread; }

	private:
		const mo_name_t		f_name;
		moBaseSPtr		f_data;
		moBaseSPtr		f_result;
		moRunnerSPtr		f_runner;
		moThreadSPtr		f_thread;
	};

	typedef moSmartPtr<moEventDeath>	moEventDeathSPtr;

	typedef unsigned int		run_count_t;
	static const run_count_t	RUN_COUNT_FOREVER = static_cast<run_count_t>(-1);
	static const run_count_t	RUN_COUNT_STOP = static_cast<run_count_t>(0);
	static const run_count_t	RUN_COUNT_ONCE = static_cast<run_count_t>(1);
	static const run_count_t	RUN_COUNT_DEFAULT = RUN_COUNT_ONCE;

				moThread(const moName& name, const moRunner *runner);
	virtual			~moThread();

	virtual const char *	moGetClassName(void) const;

	static bool		ThreadingAvailable(void);

	mo_name_t		GetName(void) const;
	virtual bool		IsRunning(void) const;
	bool			Start(run_count_t run_count = RUN_COUNT_DEFAULT);
	void			Stop(bool wait = false);
	bool			Continue(void) const;

	void			AddDeathEventPipe(const moEventPipe& event_pipe);
	void			RemoveDeathEventPipe(const moEventPipe& event_pipe);
	void			PostToDeathEventPipes(moBase *result) const;

private:
#ifdef MO_THREAD
	// internal function to start the runner
	friend void *		__internal_start(void *thread);
	void			InternalRun();
#endif

	const mo_name_t		f_name;
	mbool_t			f_running;
	mbool_t			f_wait_ended;
	mbool_t			f_is_wait_started_required;
	run_count_t		f_run_count;
	mutable moMutex		f_mutex;
	mutable moMutex		f_death_mutex;
	moMutex			f_started_signal;
	moSignal		f_ended_signal;
	moListOfEventPipes	f_death_event_pipes;
	moRunnerSPtr		f_runner;
#ifdef MO_THREAD
	pthread_t		f_thread;
	pthread_attr_t		f_thread_attr;
#endif
};




class MO_DLL_EXPORT moTaskManager : public moThread::moRunner
{
public:
	class MO_DLL_EXPORT moTask : public moBase
	{
	public:
		enum state_t {
			MO_TASK_STATE_STARTING = 0,	// this task is being created
			MO_TASK_STATE_RUNNING,		// this task is running (i.e. Run() is being called)
			MO_TASK_STATE_SLEEPING,		// this task is dormant (i.e. Run() could be called but is not)
			MO_TASK_STATE_ZOMBIE		// this taks is in the process of being removed
		};

					moTask(const moName& name);
		virtual			~moTask();

		virtual const char *	moGetClassName(void) const;

		virtual void		Run(moTaskManager *tasks_manager) = 0;

		void			SetState(state_t state);
		state_t			GetState(void) const;
		void			SetOrder(int order);
		int			GetOrder(void) const;
		void			SetPriority(int priority);
		int			GetPriority(void) const;
		bool			NeedSorting(void) const;
		mo_name_t		GetName(void) const;

		virtual compare_t	Compare(const moBase& object) const;

		// the following can be viewed as private calls
		void			Attach(void);
		void			Detach(void);

	private:
		state_t			f_state;	// this task current state
		zbool_t			f_need_sorting;
		zint32_t		f_order;	// order in which tasks are executed
		zint32_t		f_priority;	// preemptive priority -- if not sleeping higher always runs
		const mo_name_t		f_name;
		mo_atomic_word_t	f_attached;	// whether we are attached to a task manager
	};

	typedef moSmartPtr<moTask>			moTaskSPtr;
	typedef moTmplList<moTask, moSortedList>	moSortedListOfTasks;

				moTaskManager(const moName& name);
				~moTaskManager();

	virtual const char *	moGetClassName(void) const;

	virtual bool		IsRunning(void) const;

	// Tasks are run one after another;
	// A task manager runs all the tasks until Stop() is called;
	// It is expected that tasks end often;
	void			AddTask(moTask& task);
	void			RemoveTask(moTask& task);

private:
	virtual bool		Init(void);
	virtual bool		Run(void);
	virtual void		Cleanup(void);

	mutable moMutex		f_mutex;
	moSortedListOfTasks	f_tasks;
	moList::position_t	f_idx_task;
};

typedef moSmartPtr<moTaskManager>	moTaskManagerSPtr;



};		// namespace molib

// vim: ts=8 sw=8
#endif		// #ifndef MO_THREAD_H

