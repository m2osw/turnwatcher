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



#ifndef MO_APPLICATION_H
#define MO_APPLICATION_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_PROPS_H
#include "mo_props.h"
#endif
#ifndef MO_GETOPT_H
#include "mo_getopt.h"
#endif


namespace molib
{


// Could be defined in some .async file, it just is easier if it is here
class MO_DLL_EXPORT moApplicationEvent
{
public:
	virtual			~moApplicationEvent();
	virtual void		Terminate(void) = 0;
	static void		PostTerminate(molib::moEventPipeBroadcast *__broadcast);
};



class moApplication;
typedef moSmartPtr<moApplication>	moApplicationSPtr;


class MO_DLL_EXPORT moApplication : public moEventPipeBroadcast
{
public:
	class moWaitHandler : public moBase
	{
	public:
		virtual			~moWaitHandler();
		virtual void		Wait(void) = 0;
		virtual void		Done(void);
		virtual compare_t	Compare(const moBase& object) const;
	};

	typedef moTmplList<moWaitHandler, moList>	moListOfWaitHandlers;
	typedef moSmartPtr<moWaitHandler>		moWaitHandlerSPtr;

	static moApplicationSPtr Instance(void);
	static void		Done(void);

	void			SetName(const moWCString& name);
	const moWCString&	GetName(void) const;
	const moWCString&	GetFilename(void) const;

	void			SetVersion(const moWCString& version);
	const moWCString&	GetVersion(void) const;
	void			SetCopyright(const moWCString& copyright);
	const moWCString&	GetCopyright(void) const;

	void			SetRootPath(const moWCString& root_path);
	const moWCString&	GetRootPath(void) const;
	void			SetApplicationPath(const moWCString& application_path);
	const moWCString	GetApplicationPath(bool docs = false);
	const moWCString&	GetUserPath(void);
	const moWCString&	GetPrivateUserPath(bool append_version = true);

	// RDB: Added this to allow resetting of path--this makes migrating from version
	// x.1 to version x.2, for example
	//
	void			ResetUserPath();
	void			ResetPrivateUserPath();

	void			SetManufacturer(const moWCString& name);
	const moWCString&	GetManufacturer(void) const;

	void			AddWaitHandler(const moWaitHandler& handler);
	void			RemoveWaitHandler(const moWaitHandler& handler);

	void			SetOptions(const moWCString& accept, int argc, const char *argv[], bool use_default_options = true);
	moGetOpt::moOptionSPtr	Option(const moWCString& name) const;
	moGetOpt::moOptionSPtr	Option(unsigned long identifier) const;
	moGetOptSPtr		GetOpt(void) const;

	moPropBagRef		GetConfiguration(void);
	int			LoadPropBag(const moWCString& name, moPropBagRef prop_bag);

	int 			Run(void);
	void			DispatchAll(void);
	void			Terminate(bool force = false);

	virtual const char *	moGetClassName(void) const;

private:
				// singleton
				moApplication(void);
	virtual			~moApplication();

				// forbid copies
				moApplication(const moApplication& app)
					: moEventPipeBroadcast(app),
					  f_configuration("unused")
				{}
	moApplication&		operator = (const moApplication& app) { return *this; }

	void			CallWaitHandlers(void);
	void			CallDoneHandlers(void);
	void			DefineApplicationFilename(void);

	static moApplication *		g_instance;
	static zbool_t			g_done;

	mutable moMutex			f_handlers_mutex;
	moListOfWaitHandlers		f_handlers;
	zbool_t				f_run;
	moGetOptSPtr			f_getopt;
	moPropBagRef			f_configuration;
	moWCString			f_configuration_filename;
	moWCString			f_application_name;
	moWCString			f_application_version;
	moWCString			f_application_copyright;
	moWCString			f_application_filename;
	moWCString			f_root_path;
	moWCString			f_application_path;
	moWCString			f_user_path;
	moWCString			f_private_user_path;
	moWCString			f_manufacturer;
};


};		// namespace molib;

// vim: ts=8
#endif // #ifndef MO_APPLICATION_H

