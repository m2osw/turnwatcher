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




#pragma once

#include "common.h"

#include <string>
#include <vector>
#include <map>

#include "character.h"
#include "DuplicateRoll.h"

#include "mo/mo_props.h"

namespace Initiative
{

class DuplicateRoll::DuplicateResolver
{
public:
	typedef std::shared_ptr<DuplicateResolver> Pointer;

	DuplicateResolver();
	virtual ~DuplicateResolver();

	void			Clear();
	void			AddRoll( const int roll, Combatant::Character::Pointer ch );
	bool			FindCharacter( Combatant::Character::Pointer ch );
	void			RemoveCharacter( Combatant::Character::Pointer ch );
	bool			IsEmpty() const;

	void			GetOrderedVector( Combatant::Character::List& charList );

	void			Copy( DuplicateResolver::Pointer resolver );
	void			Load( molib::moPropBagRef& propBag );
	void			Save( molib::moPropBagRef& propBag );

private:
	typedef std::map<int, DuplicateRoll::Pointer>	RollToDupMap;
	RollToDupMap		f_rollMap;
};

}
// namespace Initiative

// vim: ts=8 sw=8


