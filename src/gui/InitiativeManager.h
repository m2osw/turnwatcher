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
#include "mo/mo_string.h"

// local
//
#include "character.h"
#include "DuplicateResolver.h"
#include "AppSettings.h"
#include "transaction.h"

namespace Initiative
{

struct Move
{
	typedef std::vector<Move>			List;

	Move( int pos, Combatant::Character::Pointer ch ) :
		f_newPos(pos), f_char(ch) {}

	int									f_newPos;
	Combatant::Character::Pointer		f_char;
};


struct InitiativeData
{
	typedef std::shared_ptr<InitiativeData>		Pointer;

	InitiativeData() : f_resolver( new DuplicateRoll::DuplicateResolver ) {}
	void Copy( InitiativeData::Pointer data );

	Move::List									f_movesList;
	DuplicateRoll::DuplicateResolver::Pointer	f_resolver;
	Combatant::Character::List					f_charList;		// Reference only...
};


class InitiativeManager
{
	public:
		typedef std::shared_ptr<InitiativeManager> Pointer;

										InitiativeManager();

		bool							InRounds()		const	{ return (bool) (int) f_inRoundsProp			; }
		int								RoundNumber()	const	{ return f_roundNumberProp						; }
		int								CurrentInit()	const	{ return f_currentInitProp						; }

		void							InRounds   (bool val );
		void							RoundNumber(int	 val );
		void							CurrentInit(int	 val );	

		// Clears out the current list and adds all new characters
		//
		void							AddCharacters( const Combatant::Character::List& charList );
		void							UpdateCharacters( const Combatant::Character::List& charList );

		// Access the character list
		//
		const Combatant::Character::List&	GetCharacterList() const	{ return f_initData->f_charList; }
        Combatant::Character::List&		GetCharacterList()			{ return f_initData->f_charList; }

		// Manage the character list
		//
		bool							MoveCharacter( Combatant::Character::Pointer ch, const int new_pos, const bool signal = true );
		void							RemoveCharacterMove( Combatant::Character::Pointer ch );
		void							Clear();
		void							Copy( InitiativeManager::Pointer initMgr );

		// Initiative control
		//
		void							Start( const bool signal = true );
		void							Stop( const bool signal = true );
		int								PeekNext( int* round_num ) const;
		void							Next( const int nextInit, const int roundNum, const bool signal = true );
		int								FindInitiativeOrder( Combatant::Character::Pointer ch ) const;
		Combatant::Character::Pointer	GetInitiativeCharacter( const int init_pos ) const;
		void							SetPositions( const bool signal = true );

		InitiativeData::Pointer			GetInitData()							    { return f_initData; }
		void							SetInitData( InitiativeData::Pointer data )	{ f_initData = data; }

		// Persistence
		//
	void				Load( molib::moPropBagRef& initBag );
	void				Save( molib::moPropBagRef& initBag );


		// Signal typedefs
		//
		typedef sigc::signal<void>		VoidSignal;
		typedef sigc::signal<void,int>	IntSignal;

		// Signals
		//
		VoidSignal						signal_changed()		{ return f_changedSignal;		}
		VoidSignal						signal_start_rounds()	{ return f_startRounds;			}
		VoidSignal						signal_end_rounds()		{ return f_endRounds;			}
		IntSignal						signal_turn_changed()	{ return f_turnChangedSignal;	}

	private:
		// Properties
		//
		molib::moPropIntRef				f_inRoundsProp	 	;
		molib::moPropIntRef				f_roundNumberProp	;
		molib::moPropIntRef				f_currentInitProp	;
		//
		VoidSignal						f_changedSignal;
		VoidSignal						f_startRounds;
		VoidSignal						f_endRounds;
		IntSignal						f_turnChangedSignal;
		//
		InitiativeData::Pointer			f_initData;

		// This adds the character without rebuilding the vector
		//
		Combatant::Character::Pointer	FindCharacter( const molib::moWCString& name );
		void							InternalAddCharacter( Combatant::Character::Pointer ch );
		void							InternalMoveCharacter( Combatant::Character::Pointer ch, const int new_pos );
		bool							InternalFindCharacter( Combatant::Character::Pointer ch );
		void							InternalRemoveCharacter( Combatant::Character::Pointer ch );
		void							ClearForceMoves();
		void							AdjustMoves( Combatant::Character::Pointer ch );

		// Update positions in initiative
		//
		void							ApplyMoves();
};

}
// namespace Initiative


// vim: ts=4 sw=4 noet syntax=cpp.doxygen

