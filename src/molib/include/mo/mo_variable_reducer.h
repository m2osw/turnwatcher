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



#ifndef MO_VARIABLE_REDUCER_H
#define	MO_VARIABLE_REDUCER_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_STRING_H
#include	"mo_string.h"
#endif
#ifndef MO_LIST_H
#include	"mo_list.h"
#endif



namespace molib
{


class MO_DLL_EXPORT moVariableReducer : public moBase
{
public:
	static const mowc::wc_t	MO_VR_EOF = -1;

	class MO_DLL_EXPORT moVariableReducerCommand : public moBase
	{
	public:
		enum mo_vr_param_t {
			MO_VR_PARAM_ERROR   = -1,	// index out of bounds or some other error
			MO_VR_PARAM_ANY     = 0,	// anything is fine
			MO_VR_PARAM_INTEGER = 1,	// an integer (vs a float)
			MO_VR_PARAM_FLOAT   = 2,	// a floating point value
			MO_VR_PARAM_NUMBER  = 3,	// either an integer or a float
			MO_VR_PARAM_STRING  = 4		// not a number
		};

		virtual int			Execute(const moListOfWCStrings& parameters, moWCString& result) = 0;
		virtual const moWCString	Name(void) const = 0;
		virtual	unsigned long		MinimumParameterCount(void) const;
		virtual	unsigned long		MaximumParameterCount(void) const;
		virtual mo_vr_param_t		ParameterType(unsigned long index) const;

	private:
		virtual compare_t		Compare(const moBase& object) const;
	};

	typedef moSmartPtr<moVariableReducerCommand> moVariableReducerCommandSPtr;
	typedef moTmplList<moVariableReducerCommand, moSortedListUnique> moSortedListUniqueOfVariableReducerCommand;

				moVariableReducer(void);
	virtual			~moVariableReducer();

	virtual int		Reduce(moWCString& result);
	virtual moWCString	GetVariable(const moWCString& name);
	virtual void		Undefined(const moWCString& name, bool command = false);
	virtual mowc::wc_t	GetC(unsigned long depth) = 0;

	bool			RegisterCommand(const moVariableReducerCommand& command);
	bool			UnregisterCommand(const moVariableReducerCommand& command);
	int			Command(const moWCString& command, const moListOfWCStrings& parameters, moWCString& result);

private:
	void			Reducing(moWCString& result);
	mowc::wc_t		SkipSpaces(void);
	mowc::wc_t		ReadWord(mowc::wc_t c, moWCString& word, bool accept_spaces);

	mowc::wc_t		f_unget;
	unsigned long		f_depth;
	moSortedListUniqueOfVariableReducerCommand f_commands;
};


};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_VARIABLE_REDUCER_H

