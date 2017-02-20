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



#ifndef MO_VARIABLE_H
#define	MO_VARIABLE_H
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


class MO_DLL_EXPORT moVariable : public moWCString
{
public:
				moVariable(const moWCString& name);

	moVariable&		operator = (const moWCString& value);

	// to order variables in lists
	virtual compare_t	CompareName(const moBase& object) const;

	const moWCString&	Name(void) const;

private:
				// no copy operator
				moVariable(const moVariable& variable);

	const moWCString	f_name;
};

typedef moSmartPtr<moVariable>				moVariableSPtr;
typedef moTmplList<moVariable, moSortedListUnique>	moSortedListUniqueVariable;



class MO_DLL_EXPORT moVariableList : protected moSortedListUniqueVariable
{
public:
				moVariableList(const moWCString& name);
				moVariableList(const moVariableList& list);

	//moVariableList&		operator = (const moVariableList& list);

	moVariableList&		operator += (const moVariableList& list);
	moVariableList&		operator += (const moVariable& variable);
	moVariableList&		operator += (const moWCString& set);

	virtual compare_t	Compare(const moBase& object) const;
	const moWCString&	Name(void) const;

	unsigned long		Count(void) const;
	virtual void		Append(const moVariableList& list);
	virtual void		SafeAppend(const moVariableList& list);
	moVariableSPtr		Set(const moVariable& variable);
	moVariableSPtr		Set(const moWCString& set);
	moVariableSPtr		Set(const moWCString& name, const moWCString& value);
	moVariableSPtr		Get(const moWCString& name) const;
	moVariableSPtr		Get(position_t position) const;
	void			Unset(const moWCString& name);

	/* load/save variables from/in a file */
	void			LoadFromString(const moWCString& variables, mowc::wc_t separator = '\n', mowc::wc_t append = '\0');
	bool			Load(const moWCString& filename);
	void			SaveToString(moWCString& variables, mowc::wc_t separator = '\n');
	bool			Save(const moWCString& filename);

	bool			IsEmpty(void) const;
	virtual void		Empty(void);

private:
	const moWCString	f_name;
};

typedef moSmartPtr<moVariableList>	moVariableListSPtr;






};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_VARIABLE_H

