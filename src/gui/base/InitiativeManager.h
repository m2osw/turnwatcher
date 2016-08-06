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

// stl
//
#include <string>
#include <vector>
#include <map>
#include <memory>

// molib
//
#include "mo/mo_props.h"

// local
//
#include "base/character.h"
#include "base/DuplicateResolver.h"
#include "base/AppSettings.h"
#include "base/transaction.h"

namespace Initiative
{

struct Move
{
	typedef std::vector<Move>			list_t;

	Move( int pos, Combatant::Character::pointer_t ch ) :
		f_newPos(pos), f_char(ch) {}

	int									f_newPos;
	Combatant::Character::pointer_t		f_char;
};


struct InitiativeData
{
	typedef std::shared_ptr<InitiativeData>		pointer_t;

	InitiativeData() : f_resolver( new DuplicateRoll::DuplicateResolver ) {}
	void Copy( InitiativeData::pointer_t data );

	Move::list_t									f_movesList;
	DuplicateRoll::DuplicateResolver::pointer_t	f_resolver;
	Combatant::Character::list_t					f_charList;		// Reference only...
};


class InitiativeManager
{
public:
	typedef std::weak_ptr<InitiativeManager> pointer_t;

	static pointer_t				Instance();
	static void						Release();

	bool							InRounds()		const	{ return (bool) (int) f_inRoundsProp			; }
	int								RoundNumber()	const	{ return f_roundNumberProp						; }
	int								CurrentInit()	const	{ return f_currentInitProp						; }

	void							InRounds   (bool val );
	void							RoundNumber(int	 val );
	void							CurrentInit(int	 val );	

	// Clears out the current list and adds all new characters
	//
	void							AddCharacters( const Combatant::Character::list_t& charList );
	void							UpdateCharacters( const Combatant::Character::list_t& charList, const bool emit_signal = true );

	// Access the character list
	//
	const Combatant::Character::list_t&	GetCharacterList() const	{ return f_initData->f_charList; }
	Combatant::Character::list_t&		GetCharacterList()			{ return f_initData->f_charList; }

	// Manage the character list
	//
	bool							MoveCharacter( Combatant::Character::pointer_t ch, const int new_pos, const bool signal = true );
	void							RemoveCharacterMove( Combatant::Character::pointer_t ch );
	void							Clear();

	// Initiative control
	//
	void							Start( const bool signal = true );
	void							Stop( const bool signal = true );
	int								PeekNext( int* round_num ) const;
	void							Next( const int nextInit, const int roundNum, const bool signal = true );
	int								FindInitiativeOrder( Combatant::Character::pointer_t ch ) const;
	Combatant::Character::pointer_t	GetInitiativeCharacter( const int init_pos ) const;
	void							SetPositions( const bool signal = true );

	InitiativeData::pointer_t		GetInitData()							    { return f_initData; }
	void							SetInitData( InitiativeData::pointer_t data )	{ f_initData = data; }

	// Persistence
	//
    void							LoadInitData( molib::moPropBagRef& initBag );
    bool							Load();
    bool							Save();

	// Signal typedefs
	//
	typedef sigc::signal<void>		void_signal_t;
	typedef sigc::signal<void,int>	turn_changed_signal_t;

	// Signals
	//
	void_signal_t					signal_changed()		{ return f_changedSignal;		}
	void_signal_t					signal_start_rounds()	{ return f_startRounds;			}
	void_signal_t					signal_end_rounds()		{ return f_endRounds;			}
	turn_changed_signal_t			signal_turn_changed()	{ return f_turnChangedSignal;	}

private:
	typedef std::shared_ptr<InitiativeManager> private_pointer_t;
	static private_pointer_t f_instance;

	// Properties
	//
	molib::moPropIntRef				f_inRoundsProp	 	;
	molib::moPropIntRef				f_roundNumberProp	;
	molib::moPropIntRef				f_currentInitProp	;
	molib::moName					f_mainBagName;
	//
	void_signal_t					f_changedSignal;
	void_signal_t					f_startRounds;
	void_signal_t					f_endRounds;
	turn_changed_signal_t			f_turnChangedSignal;
	//
	InitiativeData::pointer_t			f_initData;

	// Singleton only
	//
	InitiativeManager();
    InitiativeManager( const InitiativeManager& );
    InitiativeManager& operator =( const InitiativeManager& );

	// This adds the character without rebuilding the vector
	//
	Combatant::Character::pointer_t	FindCharacter( const Glib::ustring& name );
	void							InternalAddCharacter( Combatant::Character::pointer_t ch );
	void							InternalMoveCharacter( Combatant::Character::pointer_t ch, const int new_pos );
	bool							InternalFindCharacter( Combatant::Character::pointer_t ch );
	void							InternalRemoveCharacter( Combatant::Character::pointer_t ch );
	void							ClearForceMoves();
	void							AdjustMoves( Combatant::Character::pointer_t ch );

	// Update positions in initiative
	//
	void							ApplyMoves();
};

}
// namespace Initiative


// vim: ts=4 sw=4 noet syntax=cpp.doxygen

