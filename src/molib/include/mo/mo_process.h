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



#ifndef MO_PROCESS_H
#define	MO_PROCESS_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_NAME_H
#include	"mo_name.h"
#endif
#ifdef WIN32
#include	"details/mo_win32_handle.h"
#endif



namespace molib
{



class MO_DLL_EXPORT moProcess : public moBase
{
public:
				moProcess(const moName& name);
	virtual			~moProcess();

	virtual const char *	moGetClassName(void) const;

	mo_name_t		GetName(void) const;
	virtual bool		IsRunning(void) const;
	bool			Start(const moWCString& command, bool wait_for_completion = true);
	void			Stop(bool wait = true);
	bool			Wait(int msec);
	void			Detach(bool detached = true);
	int			ExitCode() const;
	pid_t			GetProcessID() const;

private:
	const mo_name_t		f_name;
	zbool_t			f_running;
	zbool_t			f_detached;
	zint32_t		f_exit_code;
	pid_t			f_process_id;
#ifdef WIN32
	mo_win32_null_handle_t	f_process_handle;
#endif
};










};		// namespace molib

// vim: ts=8 sw=8
#endif		// #ifndef MO_PROCESS_H

