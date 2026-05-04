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

#include "InitiativeManager.h"
#include "stat.h"
#include "character.h"
#include "transactions/MoveCharacterInInitiative.h"

using namespace molib;
using namespace Attribute;
using namespace Application;
using namespace Combatant;

namespace Initiative
{


InitiativeManager::private_pointer_t	InitiativeManager::f_instance;


InitiativeManager::InitiativeManager()
	: f_inRoundsProp	(moName("IN_ROUNDS"))
	, f_roundNumberProp	(moName("ROUND_NUMBER"))
	, f_currentInitProp	(moName("CURRENT_INIT"))
	, f_mainBagName		("INITIATIVE")
	, f_initData        (new InitiativeData)
{
	f_inRoundsProp		.NewProp();
	f_roundNumberProp	.NewProp();
	f_currentInitProp	.NewProp();

    Load();
}


InitiativeManager::pointer_t InitiativeManager::Instance()
{
	if( !f_instance )
	{
		f_instance.reset( new InitiativeManager );
	}
	return f_instance;
}


void InitiativeManager::Release()
{
	f_instance.reset();
}


void InitiativeManager::InRounds( bool val )
{
	f_inRoundsProp = (int) val;
}


void InitiativeManager::RoundNumber( int val )
{
	f_roundNumberProp = val;
}


void InitiativeManager::CurrentInit( int val )
{
	if( val != f_currentInitProp )
	{
		f_currentInitProp =  val;
	}
}


void InitiativeManager::AddCharacters( const Combatant::Character::list_t& charList )
{
	Clear();

	for( auto ch : charList )
	{
		InternalAddCharacter( ch );
	}

	// Cause signals to be propagated
	//
	SetPositions();
	f_changedSignal.emit();
}


void InitiativeManager::UpdateCharacters( const Combatant::Character::list_t& charList, const bool emit_signal )
{
#if defined(DEBUG) && defined(TRACE)
	printf( "InitiativeManager::UpdateCharacters(): Delete pending characters\n" );
#endif
	{
		ApplyMoves();
		//
		for( auto ch : f_initData->f_charList )
		{
			if( ch )
			{
#if defined(DEBUG) && defined(TRACE)
				printf( "InitiativeManager::UpdateCharacters(): Examine character %s, deleted=%d\n",
						ch->name().c_str(), ch->deleted() );
#endif
				if( ch->deleted() )
				{
					AdjustMoves( ch );
					InternalRemoveCharacter( ch );
					RemoveCharacterMove( ch );
				}
			}
		}
	}
#if defined(DEBUG) && defined(TRACE)
	printf( "InitiativeManager::UpdateCharacters(): Add pending characters\n" );
#endif
	{
		ApplyMoves();
		//
		for( auto ch : charList )
		{
			if( !InternalFindCharacter( ch ) )
			{
				InternalAddCharacter( ch );
			}
#if defined(DEBUG) && defined(TRACE)
			else
			{
				printf( "Character already in list: %s\n", ch->name().c_str() );
			}
#endif
		}
	}

	// Cause signals to be propagated if requested
	//
	SetPositions( emit_signal );
	//
	if( emit_signal )
	{
		f_changedSignal.emit();
	}
}


void InitiativeManager::Clear()
{
	ClearForceMoves();

	// Clear out everything
	//
	f_initData->f_resolver->Clear();
	f_initData->f_movesList.clear();
}


void InitiativeManager::ClearForceMoves()
{
	// Remove all of the force positions so it will look normal in the UI
	//
	for( auto ch : f_initData->f_charList )
	{
		ch->setForcePosition( -1 );
	}
}


void InitiativeManager::AdjustMoves( Combatant::Character::pointer_t to_delete )
{
	const int pos_base = to_delete->getPosition();
	//
	for( auto& ml : f_initData->f_movesList )
	{
		if( ml.f_newPos >= pos_base )
		{
			ml.f_newPos -= 1;
		}
	}
}


bool InitiativeManager::MoveCharacter( Combatant::Character::pointer_t ch, const int new_pos, const bool signal )
{
	if( new_pos < 0 || new_pos >= (int) f_initData->f_charList.size() )
	{
#if defined(DEBUG) && defined(TRACE)
		printf( "**** REQUEST character %s move to %d DENIED: out of range!\n", ch->name().c_str(), new_pos );
#endif
		return false;
	}

#if defined(DEBUG) && defined(TRACE)
	printf( "**** REQUEST character %s move to %d\n", ch->name().c_str(), new_pos );
#endif
	RemoveCharacterMove( ch );
	InternalMoveCharacter( ch, new_pos );
	//
	SetPositions( signal );

	// Successful move
	//
	return true;
}


void InitiativeData::Copy( InitiativeData::pointer_t data )
{
	f_resolver->Copy( data->f_resolver );
	f_charList  = data->f_charList;
	f_movesList = data->f_movesList;
}


void InitiativeManager::InternalAddCharacter( Combatant::Character::pointer_t ch )
{
	const mo_name_t initId = StatManager::Instance().lock()->initId();
	const int roll = ch->getRoll( initId );
#if defined(DEBUG) && defined(TRACE)
	printf( "**** InitiativeManager::InternalAddCharacter(): name=%s, init roll=%d\n", ch->name().c_str(), roll );
#endif
	f_initData->f_resolver->AddRoll( roll, ch );
}


void InitiativeManager::InternalMoveCharacter( Combatant::Character::pointer_t ch, const int new_pos )
{
#if defined(DEBUG) && defined(TRACE)
	printf( "**** InitiativeManager::InternalMoveCharacter(): name=%s, new_pos=%d\n", ch->name().c_str(), new_pos );
#endif
	f_initData->f_movesList.push_back( Move( new_pos, ch ) );
}


bool InitiativeManager::InternalFindCharacter( Combatant::Character::pointer_t ch )
{
	return f_initData->f_resolver->FindCharacter( ch );
}


void InitiativeManager::InternalRemoveCharacter( Combatant::Character::pointer_t ch )
{
	f_initData->f_resolver->RemoveCharacter( ch );
}


void InitiativeManager::RemoveCharacterMove( Combatant::Character::pointer_t ch )
{
	auto		iter = f_initData->f_movesList.begin();
	const auto	end  = f_initData->f_movesList.end();
	//
	for( ; iter != end; ++iter )
	{
		Move& ml = *iter;
		if( ml.f_char->name() == ch->name() )
		{
			f_initData->f_movesList.erase( iter );
			break;
		}
	}
}


void InitiativeManager::ApplyMoves()
{
	f_initData->f_resolver->GetOrderedVector( f_initData->f_charList );

	ClearForceMoves();

	for( auto ml : f_initData->f_movesList )
	{
		Combatant::Character::pointer_t ch( ml.f_char );
		if( ch->deleted() )
		{
			// Ignore deleted characters
			//
#if defined(DEBUG) && defined(TRACE)
			printf( "InitiativeManager::ApplyMoves(): ignoring deleted character %s\n",
					ch->name().c_str() );
#endif
			continue;
		}

		const int newPos = ml.f_newPos;
		//
		ch->setForcePosition( newPos );
		//
		auto		iter = f_initData->f_charList.begin();
		const auto	end  = f_initData->f_charList.end();
		//
		for( ; iter != end; ++iter )
		{
			Combatant::Character::pointer_t iter_ch = *iter;
			if( ch->name() == iter_ch->name() )
			{
				// Remove the character from the list
				//
#if defined(DEBUG) && defined(TRACE)
				printf( "InitiativeManager::ApplyMoves(): removing character %s from list\n", ch->name().c_str() );
#endif
				f_initData->f_charList.erase( iter );
				break;
			}
		}
		//
		if( newPos >= f_initData->f_charList.size() )
		{
#if defined(DEBUG) && defined(TRACE)
			printf( "InitiativeManager::ApplyMoves(): character %s moved to position %d (end)\n", ch->name().c_str(), f_initData->f_charList.size() );
#endif
			f_initData->f_charList.push_back( ch );
		}
		else
		{
			int pos = 0;
			iter = f_initData->f_charList.begin();
			for( ; iter != end; ++pos, ++iter )
			{
				if( pos == newPos )
				{
					// Now insert the character at the new position
					//
#if defined(DEBUG) && defined(TRACE)
					printf( "InitiativeManager::ApplyMoves(): character %s moved to position %d\n", ch->name().c_str(), pos );
#endif
					f_initData->f_charList.insert( iter, ch );
					break;
				}
			}
		}
	}
}


void InitiativeManager::SetPositions( const bool signal )
{
	// Apply moves to character list
	//
	ApplyMoves();

	// Set the positions and emit signal if required
	//
	int pos = 0;
	//
	for( auto ch : f_initData->f_charList )
	{
		ch->setPosition( pos );
		if( signal )
		{
			ch->signal_changed().emit();
		}
        ++pos;
	}

#if defined(DEBUG) && defined(TRACE)
	printf( "**** Character::list_t after position change:\n" );
	for( auto ch : f_initData->f_charList )
	{
		printf( "%p: character name=%s, position=%d\n", (Combatant::Character*) ch, ch->name().c_str(), ch->getPosition() );
	}
	printf( "**** End characters\n" );
#endif
}


void InitiativeManager::Start( const bool signal )
{
	f_inRoundsProp = 1;
	//
	if( signal )
	{
		f_startRounds.emit();
		f_changedSignal.emit();
	}
}


void InitiativeManager::Stop( const bool signal )
{
	f_inRoundsProp = 0;
	//
	if( signal )
	{
		f_endRounds.emit();
		f_changedSignal.emit();
	}
}


void InitiativeManager::Next( const int nextInit, const int roundNum, const bool signal )
{
	CurrentInit( nextInit );
	RoundNumber( roundNum );

	if( signal )
	{
		f_turnChangedSignal.emit( nextInit );
		f_changedSignal.emit();
	}
}


int InitiativeManager::PeekNext( int* round_num ) const
{
	const int		count			= f_initData->f_charList.size();
	int				idx				= 0;
	int				nextInit		= CurrentInit();
	bool			skip			= true;
	//
	auto appSettings( AppSettings::Instance().lock() );
	assert(appSettings);
	//
	// Prevent deadlock when all characters are dead and SkipDead option is on
	//
	for( ; skip && idx < count; ++idx )
	{
		skip = false;
		//
		if( ++nextInit >= count )
		{
			nextInit = 0;	// Go around the horn
			if( round_num )
			{
				(*round_num)++;
			}
		}
		//
		Combatant::Character::pointer_t ch( GetInitiativeCharacter( nextInit ) );
#if defined(DEBUG) && defined(TRACE)
		printf( "InitiativeManager::PeekNext(): nextInit=%d, ch=%s, status=%s\n",
				nextInit,
				ch->name().c_str(),
				ch->status_string().c_str() );
#endif

		if( ch->health() == Combatant::Dead )
		{
			// Skip over characters
			//
			skip = appSettings->SkipDead();
#if defined(DEBUG) && defined(TRACE)
			printf( "InitiativeManager::PeekNext(): Dead char found, skipping!\n" );
#endif
		}
	}

	return nextInit;
}


namespace
{
	class MoveLoader
	{
		public:
			MoveLoader() :
				f_newPos( moName("NEWPOS") ),
				f_char  ( moName("NAME") )
			{
				f_newPos.NewProp();
				f_char.NewProp();
			}

			void NewPos( const int pos )				{ f_newPos = pos; }
			void Character( const QString& name )	{ f_char = name.c_str(); }

			int NewPos() const							{ return f_newPos; }
			const QString	Character() const	    { return static_cast<moWCString>(f_char).c_str(); }

			void Load( moPropBagRef& propBag )
			{
				f_newPos.Link( propBag ); if( !f_newPos.HasProp() ) f_newPos.NewProp();
				f_char  .Link( propBag ); if( !f_char  .HasProp() ) f_char	.NewProp();
			}

			void Save( moPropBagRef& propBag )
			{
				propBag += f_newPos;
				propBag += f_char;
			}

		private:
			moPropIntRef		f_newPos;
			moPropStringRef	f_char  ;
	};
}


Combatant::Character::pointer_t InitiativeManager::FindCharacter( const QString& name )
{
	auto charMgr = CharacterManager::Instance().lock();
	assert(charMgr);
	return charMgr->FindCharacter( name );
}


int InitiativeManager::FindInitiativeOrder( Combatant::Character::pointer_t ch ) const
{
	int idx = 0;
	for( auto search_char : f_initData->f_charList )
	{
		if( search_char->name() == ch->name() )
		{
			break;
		}
		//
		++idx;
	}

	return idx;
}


Combatant::Character::pointer_t InitiativeManager::GetInitiativeCharacter( const int init_pos ) const
{
	const int						curr_init  = (f_initData->f_charList.size() - 1) - init_pos;
	Combatant::Character::pointer_t	init_char  = f_initData->f_charList[curr_init];
	return init_char;
}


void InitiativeManager::LoadInitData( molib::moPropBagRef& initBag )
{
	// Clear everything
	//
	f_initData->f_resolver->Clear();
	f_initData->f_movesList.clear();
	f_initData->f_charList.clear();

	// Get local properties
	//
	f_inRoundsProp	 .Link( initBag ); if( !f_inRoundsProp	 .HasProp() ) { f_inRoundsProp	 .NewProp(); }
	f_roundNumberProp.Link( initBag ); if( !f_roundNumberProp.HasProp() ) { f_roundNumberProp.NewProp(); }
	f_currentInitProp.Link( initBag ); if( !f_currentInitProp.HasProp() ) { f_currentInitProp.NewProp(); }

	// Load the resolver contents
	//
	f_initData->f_resolver->Load( initBag );

	// Now fetch the manual moves
	//
	moPropArrayRef	array("MOVES");
	array.Link( initBag );
	if( !array.HasProp() )	array.NewProp();
	//
	const int count = array.CountIndexes();
	for( int idx = 0; idx < count; ++idx )
	{
		const int		item_no  = array.ItemNoAtIndex( idx );
		moPropSPtr		prop_ptr = array.Get( item_no );
		moName			name	 = prop_ptr->GetName();
		moPropBagRef	propBag(name);
		propBag.NewProp();
		propBag.GetProperty()->Copy( *prop_ptr );
		//
		MoveLoader ml;
		ml.Load( propBag );
		auto ch = FindCharacter( ml.Character() );
		f_initData->f_movesList.push_back( Move( ml.NewPos(), ch ) );
	}

	// Apply the new ordering
	//
	SetPositions();
}


bool InitiativeManager::Load()
{
	// Load previous state
	//
	moPropBagRef mainPropBag( f_mainBagName	);
	if( !Common::LoadBagFromFile( "initiative.conf", mainPropBag ) ) return false;

	LoadInitData( mainPropBag );

	return true;
}


bool InitiativeManager::Save()
{
	// Create sub-bag
	//
	moPropBagRef mainPropBag( f_mainBagName );
	mainPropBag.NewProp();

	// Local properties
	//
	mainPropBag += f_inRoundsProp;
	mainPropBag += f_roundNumberProp;
	mainPropBag += f_currentInitProp;

	// Save the resolver contents
	//
	f_initData->f_resolver->Save( mainPropBag );

	// Save the manual moves in an array
	//
	moPropArrayRef	array("MOVES");
	array.NewProp();
	//
	int array_index = 0;
	for( auto& m : f_initData->f_movesList )
	{
		auto ch( m.f_char );
		if( !ch->deleted() )
		{
			const int newPos( m.f_newPos );
			//
			MoveLoader ml;
			ml.NewPos   ( newPos     );
			ml.Character( ch->name() );
			//
			moPropBagRef propBag( "CHARACTER" );
			propBag.NewProp();
			ml.Save( propBag );
			//
			array.Set( array_index, propBag );
		}
		++array_index;
	}
	//
	mainPropBag += array;

	// Save file
	//
	return Common::SaveBagToFile( "initiative.conf", mainPropBag );
}


}
// namespace Initiative

// vim: ts=4 sw=4 noet syntax=cpp.doxygen

