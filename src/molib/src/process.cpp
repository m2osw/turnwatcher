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
#pragma implementation "mo/mo_process.h"
#endif

#include	"mo/mo_process.h"

#ifdef WIN32
#include	"mo/details/mo_win32_handle.h"
#include	<tlhelp32.h>
#include	<vector>
#else
#include	<sys/wait.h>
#endif



namespace molib
{


namespace {
/** \brief Internal definition of an invalid process identifier.
 *
 * at this time, all systems we use use -1 as an invalid
 * process identifier. This will enable us to change that
 * depending on the system if necessary.
 */
const pid_t		INVALID_PROCESS_ID = -1;
}



#ifdef WIN32
/** \brief Function used under Win32 to kill a process.
 *
 * This function is used to kill a process and all of its children
 * (by default Win32 does NOT kill recursively, so we have to have
 * our own function.)
 *
 * The moProcess::Detach() function can be used to bypass this
 * killing spree, but only if the parent process quits before this
 * function gets called.
 *
 * \param[in] process_id Top most process to kill
 */
namespace {
void kill_children(int process_id)
{
	HANDLE			snapshot, process;
	PROCESSENTRY32W		process_entry;
	BOOL			cont;
	bool			found;
	std::vector<DWORD>	ids;

	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(snapshot == INVALID_HANDLE_VALUE) {
		// should we throw here?!
		return;
	}

	ids.push_back(process_id);

	// TODO: since nothing can be locked, this algorithm is
	//	 quite poor. One of the child process can very well
	//	 have the time to create 10 new processes before we
	//	 kill them all.
	//
	// Could we block all the processes threads to avoid
	// that problem?
	do {
		found = false;
		memset(&process_entry, 0, sizeof(process_entry));
		process_entry.dwSize = sizeof(process_entry);
		cont = Process32FirstW(snapshot, &process_entry);

		while(cont) {
			// if we have the parent in our list, make it part of the list too
			if(std::find(ids.begin(), ids.end(), process_entry.th32ParentProcessID) != ids.end()
			&& std::find(ids.begin(), ids.end(), process_entry.th32ProcessID) == ids.end()) {
				ids.push_back(process_entry.th32ProcessID);
				found = true;
			}

			memset(&process_entry, 0, sizeof(process_entry));
			process_entry.dwSize = sizeof(process_entry);
			cont = Process32NextW(snapshot, &process_entry);
		}
	} while(found);

	// Should we handle errors here?
	//DWORD err = GetLastError();
	//if(err != ERROR_NO_MORE_FILES)
	//{
	//}

	// Now kill all the processes we've found.
	std::vector<DWORD>::iterator idx = ids.begin();
	while(idx != ids.end()) {
		process = OpenProcess(
			PROCESS_TERMINATE,
			FALSE,
			*idx);
		if(process != NULL) {
			// TODO:
			//
			// this may tell us that the process already
			// died; since we want to kill it... we should
			// be just fine!
			//
			// We should look into sending a Ctrl-C instead
			// which would be catchable. And if after a
			// few milliseconds the process is still running,
			// kill it with TerminateProcess()!
			//
			// Also, molib linked children will be sent
			// a QUIT event. But until all processes are
			// molib processes...
			TerminateProcess(process, 0);
		}
		++idx;
	}
}
}		// namespace
#endif



/** \brief Initializes a process
 *
 * The constructor creates an empty process. Call Start()
 * once you are ready to run the child process.
 *
 * The name will eventually be used to send events.
 *
 * \param[in] name The name of the process object (not the command)
 *
 * \sa moProcess::Start()
 */
moProcess::moProcess(const moName& name)
	: f_name(name),
	  //f_running -- auto-init
	  //f_detach -- auto-init
	  f_exit_code(-1),
	  f_process_id(INVALID_PROCESS_ID)
	  //f_process_handle -- auto-init
{
}


/** \brief Destroy the process.
 *
 * The destructor ensures that the child process is killed before
 * it returns.
 *
 * If you want to start a process and leave it running, then
 * call the Detach() function.
 *
 * \sa moProcess::Stop()
 */
moProcess::~moProcess()
{
	// make sure the process is stopped (unless we are detatched)
	if(!f_detached) {
		Stop();
	}
	else {
		// try this so we may catch a zombie or two...
		(void) IsRunning();
	}
}


// autodoc by moBase
const char *moProcess::moGetClassName(void) const
{
	return "molib::moProcess";
}






/** \brief Return the name of this process.
 *
 * This function returns the name you specified when creating
 * the process. It can be used internally to manage your list
 * of processes and receive events from them.
 *
 * \return The name of this process.
 */
mo_name_t moProcess::GetName(void) const
{
	return f_name;
}


/** \brief Return true if the process is running
 *
 * This function checks whether the process is still running.
 * If so, it returns true. If not, it returns false.
 *
 * A Zombie process will be removed by this call.
 *
 * \return true if the process was started and is still running
 *
 * \sa moProcess::Start()
 * \sa moProcess::Stop()
 */
bool moProcess::IsRunning(void) const
{
	if(f_running) {
		// if still running, check the state the system says it's in
		// and return that instead
#ifdef WIN32
		if(WaitForSingleObject(f_process_handle, 0) != WAIT_TIMEOUT) {
			// the process did quit or died since last call
			DWORD exit_code;
			GetExitCodeProcess(f_process_handle, &exit_code);
			const_cast<moProcess *>(this)->f_exit_code = static_cast<int>(exit_code);

			const_cast<moProcess *>(this)->f_process_id     = INVALID_PROCESS_ID;
			const_cast<moProcess *>(this)->f_process_handle = NULL;
			const_cast<moProcess *>(this)->f_running        = false;
		}
#else
		int result, status;
		result = waitpid(f_process_id, &status, WNOHANG);

		if(result != 0) {
			if(WIFEXITED(status) || WIFSIGNALED(status)) {
				const_cast<moProcess *>(this)->f_process_id = INVALID_PROCESS_ID;
				const_cast<moProcess *>(this)->f_running    = false;

				if(WIFEXITED(status)) {
					const_cast<moProcess *>(this)->f_exit_code = WEXITSTATUS(status);
				}
				else {
					const_cast<moProcess *>(this)->f_exit_code = -1;
				}
			}
			else {
				// an error occured... does that mean the process is gone?
				const_cast<moProcess *>(this)->f_exit_code = -1;
			}
		}
#endif
	}

	return f_running;
}


/** \brief Start running the specified command
 *
 * This command starts the specified command. The command can be a
 * full path and be followed by parameters.
 *
 * By default, the function returns true.
 *
 * \note
 * This function calls the Stop() function with the wait flag
 * set to true. If you do not want to block waiting for older
 * processes to quit, create a new moProcess object and start
 * the command with that new object.
 *
 * \bug
 * The IsRunning() function may return true before the system
 * tries to start the new process. Be aware of that since this
 * means the GetProcessID() function can return an invalid
 * identifier even when IsRunning() returns true.
 *
 * \param[in] command The command to execute this process
 * \param[in] wait_for_completion Whether this object waits
 * 		for the process to complete before returning
 *
 * \return true if the process started successfully
 *
 * \sa moProcess::Stop()
 * \sa moProcess::IsRunning()
 */
bool moProcess::Start(const moWCString& command, bool wait_for_completion)
{
	// make sure the previous instance stopped
	Stop();

#ifdef WIN32
	STARTUPINFO		startup_info;
	PROCESS_INFORMATION	process_information;
	BOOL			result;

	// get a copy of the command in 'char*' format
	// we need this in a read/write buffer anyway!!!
	std::auto_ptr<char> cmd(command.MBData());

	memset(&startup_info, 0, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);

	// TODO: add support for pipes
	// TODO: use CreateProcessW() instead?
	f_running = true;
	result = CreateProcessA(
			NULL,
			cmd.get(),
			NULL,
			NULL,
			true,
			0,
			NULL,
			NULL,
			&startup_info,
			&process_information
		);

	if(result) {
		// we don't need the thread
		CloseHandle(process_information.hThread);

		// keep the process info
		f_process_id     = process_information.dwProcessId;
		f_process_handle = process_information.hProcess;

		if(wait_for_completion) {
			WaitForSingleObject(f_process_handle, INFINITE);

			f_process_id     = INVALID_PROCESS_ID;
			f_process_handle = NULL;
			f_running        = false;
		}
	}
	else {
		f_running = false;
	}

	return result != 0;
#else
	int		result;

	if(wait_for_completion) {
		// TODO: add support for pipes
		f_running = true;
		result = system(command.c_str());
		f_running = false;
	}
	else {
		f_process_id = fork();
		if(f_process_id == 0) {
			// mark the process as running even in the child
			// TODO: use execve()/spawn() or similar?
			// (this would be a good idea because the system()
			// call generates a 2nd fork() call!)
			f_running = true;
			result = system(command.c_str());
			f_running = false;
			exit(result);
		}
		else if(f_process_id == INVALID_PROCESS_ID) {
			return false;
		}
		else {
			f_running = true;
			return true;
		}
	}

	return result >= 0 && result <= 255;
#endif
}



/** \brief Stop the process.
 *
 * This function stops the process.
 *
 * If the process is not running, the function returns
 * immediately.
 *
 * If the process is still running, the function will
 * call the necessary system functions to terminate
 * it. If the wait parameter is set to true, the
 * function will wait for 1 second and try again
 * until the process quits. Otherwise, the stop
 * signal is sent once and the function returns
 * immediately.
 *
 * \param[in] wait Whether to wait for termination
 *
 * \sa moProcess::Start()
 * \sa moProcess::IsRunning()
 */
void moProcess::Stop(bool wait)
{
#ifndef WIN32
	// first send a SIGTERM
	int	signal = SIGTERM;
#endif

	while(IsRunning()) {
		// still running, send a kill signal
#ifdef WIN32
		// Under Windows, we want to kill all the
		// children. This means we have to recursively
		// search all the processes that this process
		// created, and its children, etc.
		kill_children(f_process_id);
#else
		kill(f_process_id, signal);
#endif

		if(!wait) {
			break;
		}
		// TODO: should we instead have a timed wait?
		// Also the user may want to be in control of the time
#ifdef WIN32
		Sleep(100);
#else
		signal = SIGKILL;
		usleep(100000);
#endif
	}
}



/** \brief Wait the process to die.
 *
 * This function blocks until this process dies.
 *
 * If you specify a number of milli-seconds, it represents the
 * maximum number of milli-seconds to wait before this function
 * returns.
 *
 * \param[in] msec The number of milli-seconds to wait before returning
 *
 * \return true if the process died, false if the wait timed out
 *
 * \sa moProcess::IsRunning()
 */
bool moProcess::Wait(int msec)
{
#ifdef WIN32
	if(f_running) {
		// under Microsoft Windows we can simply wait
		WaitForSingleObject(f_process_handle, msec);
	}
#else
	// could we use a select() somehow here?!
	// (we could also create a thread to signal us in case
	// it wakes up before the process dies...)
	int wait_usec;
	while(IsRunning() && msec > 0) {
		if(msec >= 100) {
			wait_usec = 100000;
			msec -= 100;
		}
		else {
			wait_usec = msec * 1000;
			msec = 0;
		}
		usleep(wait_usec);
	}
#endif

	return !IsRunning();
}



/** \brief Mark the process as detached
 *
 * This function is used to mark the process detached
 * (or not). A detached process continues to run whenever
 * this moProcess object is destroyed.
 *
 * However, a detached process will still be killed if you
 * call the Start() or Stop() functions.
 *
 * This is useful to start a process and leave it running
 * after your function exists as in:
 *
 * \code
 * void StartServer()
 * {
 *	moProcess server("Server");
 *	bool result = server.Start("my_server -f server.conf");
 *	server.Detach();
 *	return result;
 * }
 * \endcode
 *
 * \param[in] detached Whether the child process should be detached from its parent
 *
 * \sa moProcess::~moProcess()
 * \sa moProcess::Start()
 */
void moProcess::Detach(bool detached)
{
	f_detached = detached;
}


/** \brief Returns the process identifier
 *
 * This function returns the current process identifier.
 *
 * \return The running process identifier or -1
 *
 * \sa moProcess::Start()
 */
pid_t moProcess::GetProcessID() const
{
	return f_process_id;
}


/** \brief Get the code returned by the process.
 *
 * This function returns the code that the child process
 * returned. When valid, it is a value from 0 to 255.
 *
 * If the process is still running, an error occurs or no
 * process was ever run, then this function returns -1.
 *
 * \note
 * Start() resets this value to -1. IsRunning() sets it with the
 * actual result (Stop() calls IsRunning() as needed.)
 *
 * \return The exit code of the child process
 *
 * \sa moProcess::IsRunning()
 * \sa moProcess::Stop()
 */
int moProcess::ExitCode() const
{
	return f_exit_code;
}



};			// namespace molib;

// vim: ts=8

