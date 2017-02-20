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

#pragma once

#include "common.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "character.h"

namespace Initiative
{

/*
 * When adding a character, we only roll a tie breaker when there is exactly one
 * character already in the charList. Both objects get a roll (the one in the list
 * plus the one being added). The new rolls plus characters are added to the
 * resolver instance, then the order is extracted. The new characters are added to
 * the vector in correct order.
 * 
 * When there are two or more in the container, the new addition has one roll for
 * tie-breaking made, and is then added to the resolver object. The vector is then
 * refreshed in correct order.
*/

class DuplicateRoll
{
public:
	typedef std::shared_ptr<DuplicateRoll>	pointer_t;
	class DuplicateResolver;

	DuplicateRoll();
	virtual ~DuplicateRoll();

	void				AddCharacter( Combatant::Character::pointer_t ch );
	bool				FindCharacter( Combatant::Character::pointer_t ch );
	void				RemoveCharacter( Combatant::Character::pointer_t ch );
	bool				IsEmpty() const;
	void				GetOrderedVector( Combatant::Character::list_t& charList );

	void				Copy( DuplicateRoll::pointer_t dup_roll );
	void				Load( molib::moPropBagRef& propBag );
	void				Save( molib::moPropBagRef& propBag );

private:
	Combatant::Character::pointer_t		f_char;
	std::shared_ptr<DuplicateResolver>	f_dupResolver;
};

}
// namespace Initiative


// vim: ts=8 sw=8


