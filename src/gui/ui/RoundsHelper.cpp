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

// local
//
#include "common.h"
#include "RoundsHelper.h"
#include "DamageWindow.h"
#include "EditWindow.h"
#include "ExpiredEffectsWindow.h"
#include "InitWindow.h"
#include "InputWindow.h"
#include "JumpInWindow.h"
#include "transactions.h"
#include "stat.h"

// stl
//
#include <algorithm>

// molib
//
#include "mo/mo_name.h"

using namespace Effects;
using namespace Transactions;

namespace UI
{


RoundsHelper::RoundsHelper	( Gtk::Window* 		parent
							, CharacterView*	charView
							, HUDWindow*		hudView
                            )
	: f_parent(parent)
	, f_charView(charView)
	, f_hudView(hudView)
{
	// empty
}


RoundsHelper::~RoundsHelper()
{
	// empty
}


bool RoundsHelper::CreateNextInIntiativeTransaction( TransactionGroup::pointer_t group )
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	auto initMgr( GetInitMgr().lock() );
	assert(initMgr);

	bool okay = true;
	Transaction::pointer_t tr;
	//
	if( initMgr->PeekNext( NULL ) == 0 && appSettings->UltraInit() )
	{
		okay = RollInitiative( group, true /*allowCancel*/ );
		//
		if( okay )
		{
			tr.reset( new ResolveInitiativeDuplicates );
			tr->doit();
			group->addTransaction( tr );
		}
	}

	if( okay )
	{
		DisplayPendingEffects();
		//
		tr.reset( new NextInitTransaction() );
		tr->doit();
		group->addTransaction( tr );
		//
		if( appSettings->NotifyExpiredEffects() )
		{
			Actions::Desensitizer	ad;

            ExpiredEffectsWindow dlg( gettext("Expired effects"), *f_parent );
            dlg.set_transient_for( *f_parent );
			//
			Combatant::Character::pointer_t ch = initMgr->GetInitiativeCharacter( initMgr->CurrentInit() );
			//
			if( dlg.AddExpiredEffects( ch ) )
			{
				if( dlg.run() == Gtk::RESPONSE_YES )
				{
					Combatant::Character::pointer_t newChar( new Combatant::Character );
					newChar->Copy( ch );

					Effect::list_t	effect_list;
					newChar->getEffects(effect_list);

					Effect::list_t	newEffects;
					for( auto effect : effect_list )
					{
						assert(effect);
						if( effect->roundsLeft() > 0 )
						{
							newEffects.push_back( effect );
						}
					}

					newChar->setEffects( newEffects );
					//
					Transaction::pointer_t tr( new EditCharacterTransaction( ch, ch, newChar ) );
					tr->doit();
					group->addTransaction( tr );
				}
			}
		}
	}

	return okay;
}


void RoundsHelper::DisplayPendingEffects()
{
	auto initMgr( GetInitMgr().lock() );
	assert(initMgr);

	Actions::Desensitizer	ad;

	Combatant::Character::pointer_t nextCh = initMgr->GetInitiativeCharacter( initMgr->PeekNext( NULL ) );

	Effect::list_t	effect_list;
	nextCh->getEffects( effect_list );
	//
	Glib::ustring effect_list_str;
	//
	for( auto effect : effect_list )
	{
		assert(effect);
		if( effect->startIn() == 1 )
		{
			if( effect_list_str.size() )
			{
				effect_list_str += ", ";
			}
			effect_list_str += effect->name().c_str();
		}
	}

	if( effect_list_str.size() )
	{
		Glib::ustring message = gettext("The following effects have become active this round:\n");
		message += effect_list_str;
		Gtk::MessageDialog dlg( 
			message,
			false,
			Gtk::MESSAGE_INFO,
			Gtk::BUTTONS_OK,
			true
		);
        dlg.set_transient_for( *f_parent );
		dlg.run();
	}
}


bool RoundsHelper::RollInitiative( TransactionGroup::pointer_t group, bool allowCancel )
{
	Actions::Desensitizer	ad;

	auto statMgr( GetStatMgr().lock() );
	assert(statMgr);

	bool cancel = false;
	const molib::mo_name_t initId( statMgr->initId() );

#ifdef DEBUG
    auto debug_func = [&initId,this]( const InitWindow::CharacterPair& pair )
    {
        const int orig_roll = pair.f_origChar->getRoll( initId );
        const int new_roll  = pair.f_newChar ->getRoll( initId );
        std::cerr
                << "**** "
                <<  pair.f_origChar->name().c_str()
                << ": orig_roll=" << orig_roll
                << ", new_roll=" << new_roll
                << std::endl;
    };
#endif

	if( GetAppSettings().lock()->ManualInit() )
	{
		// Make the rolls, but don't squirrel it away into the undo group--instead we'll add
		// them to the UI
		//
		RollTransaction rt( initId );
		rt.doit();

		InitWindow::pair_t initCharPairList;
		//
		for( auto ch : GetCharacterMgr().lock()->GetCharacters() )
		{
			assert(ch);
			initCharPairList.push_back( InitWindow::CharacterPair( ch ) );
		}

		InitWindow wnd( gettext("Input Initiative Rolls"), allowCancel );
        wnd.set_transient_for( *f_parent );
		wnd.SetCharacters( initCharPairList );

		int response_id;
		while( (response_id = wnd.run()) == Gtk::RESPONSE_REJECT );
		wnd.hide();

		// undo the rolls, because we'll add them from the edit transactions below
		//
		rt.undo();

		if( response_id == Gtk::RESPONSE_OK )
		{
            for( auto& pair : initCharPairList )
			{
				Transaction::pointer_t tr( new EditCharacterTransaction( 
						pair.f_origChar, pair.f_origChar, pair.f_newChar ) );
				tr->doit();
				group->addTransaction( tr );
#ifdef DEBUG
                debug_func( pair );
#endif
            }
		}
		else
		{
			cancel = true;
		}
	}
	else
	{
		// Make the rolls
		//
		Transaction::pointer_t tr( new RollTransaction( initId ) );
		tr->doit();
		//
		group->addTransaction( tr );
	}


	return !cancel;
}


void RoundsHelper::RollInitiative()
{
	TransactionGroupHelper helper( gettext("Roll Initiative"), false /*auto_doit*/ );
	const bool start = RollInitiative( helper.GetGroup(), true /*allowCancel*/ );
	//
	if( start )
	{
		helper.AddTransaction( Transaction::pointer_t( new ResolveInitiativeDuplicates ), true /*doit*/ );
	}
	else
	{
		helper.Abort();
	}
}


void RoundsHelper::MoveCharacters( const bool up )
{
	Combatant::Character::list_t selected = f_charView->getSelected();
	const int count = (int) selected.size();

	// Sanity check
	//
	if( count == 0 ) return;

	const int BUFSIZE = 128;
	char msg[BUFSIZE+1];
	snprintf( msg, BUFSIZE, "Move Character%s %s in Initiative", count == 1? "": "s", up? "Up": "Down" );

#ifdef DEBUG
	printf( "RoundsHelper: DEBUG: %s\n", msg );
#endif

	TransactionGroupHelper helper( gettext(msg) );
	Transaction::pointer_t tr( new MoveSelectedCharacters( f_charView->getSelected(), up ) );
	helper.AddTransaction( tr );
}


void RoundsHelper::Start()
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	TransactionGroupHelper helper( gettext("Start Rounds"), false /*auto_doit*/ );

	bool start = true;
	if( appSettings->InitOnStart() )
	{
        start = RollInitiative( helper.GetGroup(), true /*allowCancel*/ );
	}

	if( start )
	{
		GetCharacterModel().lock()->setDefaultSort();
		f_hudView->SetDefaultSort();
		//
		helper.AddTransaction( Transaction::pointer_t( new StartInitTransaction()      ) , true /*doit*/ );
		helper.AddTransaction( Transaction::pointer_t( new ResolveInitiativeDuplicates ) , true /*doit*/ );
	}
	else
	{
		helper.Abort();
	}
}


void RoundsHelper::End()
{
	TransactionGroupHelper	helper( gettext("End Rounds") );
	helper.AddTransaction( Transaction::pointer_t( new ClearSortTransaction      () ) );
	helper.AddTransaction( Transaction::pointer_t( new SetDefaultSortTransaction () ) );
	helper.AddTransaction( Transaction::pointer_t( new StopInitTransaction       () ) );
}


void RoundsHelper::Next()
{
	if( !GetInitMgr().lock()->InRounds() )
	{
		return;
	}

	f_hudView->SetDefaultSort();

	TransactionGroupHelper helper( gettext("Next In Initiative Order"), false /*auto_doit*/ );
	helper.AddTransaction( Transaction::pointer_t( new SetDefaultSortTransaction() ), true /*doit*/ );
	//
	if( !CreateNextInIntiativeTransaction( helper.GetGroup() ) )
	{
		helper.Abort();
	}
}


void RoundsHelper::Delay()
{
	auto initMgr( GetInitMgr().lock() );
	assert(initMgr);

	TransactionGroupHelper helper( gettext("Delay Character"), false /*auto_doit*/ );
	helper.AddTransaction( Transaction::pointer_t( new SetDefaultSortTransaction() ), true /*doit*/ );
	helper.AddTransaction( Transaction::pointer_t( new DelayTransaction( initMgr->GetInitiativeCharacter( initMgr->CurrentInit() ) ) ), true /*doit*/ );
	//
	if( !CreateNextInIntiativeTransaction( helper.GetGroup() ) )
	{
		helper.Abort();
	}
}


void RoundsHelper::Ready()
{
	auto initMgr( GetInitMgr().lock() );
	assert(initMgr);

	TransactionGroupHelper helper( gettext("Ready an Action") );
	helper.AddTransaction( Transaction::pointer_t( new SetDefaultSortTransaction() ) );
	helper.AddTransaction( Transaction::pointer_t( new DelayTransaction( initMgr->GetInitiativeCharacter( initMgr->CurrentInit() ) ) ), Combatant::Readied );
	//
	if( !CreateNextInIntiativeTransaction(helper.GetGroup()) )
	{
		helper.Abort();
	}
}


void RoundsHelper::JumpIn()
{
	Actions::Desensitizer	ad;
    JumpInWindow jumpInWindow( gettext("Jump In"), *f_parent );

	for( auto ch : GetCharacterMgr().lock()->GetCharacters() )
	{
		assert(ch);
		if( ch->status() == Combatant::Delayed || ch->status() == Combatant::Readied )
		{
			jumpInWindow.AddCharacter( ch );
		}
	}

	if( jumpInWindow.Size() == 0 )
	{
		// Nothing delayed
		//
		return;
	}

	int response_id;
	if( jumpInWindow.Size() > 1 )
	{
		jumpInWindow.show();
		jumpInWindow.set_focus( jumpInWindow );
		response_id = jumpInWindow.run();
		jumpInWindow.hide();
	}
	else
	{
		response_id = Gtk::RESPONSE_OK;
	}

	if( response_id == Gtk::RESPONSE_OK )
	{
		Combatant::Character::pointer_t ch = jumpInWindow.GetSelected();
		TransactionGroupHelper helper( gettext("Jump In") );
		helper.AddTransaction( Transaction::pointer_t( new UndelayTransaction( ch ) ) );

		auto initMgr( GetInitMgr().lock() );
		assert(initMgr);
		Combatant::Character::pointer_t init_char( initMgr->GetInitiativeCharacter( initMgr->CurrentInit() ) );
		const int init_pos = init_char->getPosition();
#ifdef DEBUG
		printf( "****** RoundsHelper::JumpIn() init_char=%s, init_pos=%d, jump_in_char=%s, jump_in_pos=%d\n",
				init_char->name().c_str(),
				init_pos,
				ch->name().c_str(),
				ch->getPosition() );
#endif
		helper.AddTransaction( Transaction::pointer_t( new MoveCharacterInInitiative( init_pos, ch, true /*set_positions*/ ) ) );
	}
}


void RoundsHelper::Damage()
{
	Actions::Desensitizer	ad;

	Combatant::Character::list_t selected = f_charView->getSelected();
	const int count = selected.size();
    DamageWindow	damageWindow( gettext("Damage Window"), *f_parent );
	damageWindow.set_title( (count==1)? gettext("Enter Damage / Health") : gettext("Enter Mass Damage / Health") );
	//
	if( count == 1 )
	{
		damageWindow.Char( selected[0] );
	}
	else
	{
		damageWindow.Char( 0 );
	}

	const int response_id = damageWindow.run();
	damageWindow.hide();
	if( response_id != Gtk::RESPONSE_CANCEL )
	{
		Glib::ustring msg;
		//
		if( count > 1 )
		{
			msg = "Mass ";
		}
		//
		int damage = 0;
		bool full = false;
		//
		switch( response_id )
		{
			case Gtk::RESPONSE_OK:
				msg += gettext("Damage");
				damage = damageWindow.Value();
				break;

			case RESPONSE_HEALTH:
				msg += gettext("Health");
				damage = -damageWindow.Value();
				break;

			case RESPONSE_FULLHEATH:
				msg += gettext("Full Health");
				full = true;
				break;
		}

		// Add Damage/Health to all characters selected
		//
		TransactionGroupHelper helper( msg.c_str() );
		//
		for( int idx = 0; idx < count; ++idx )
		{
			Combatant::Character::pointer_t ch = selected[idx];
			helper.AddTransaction( Transaction::pointer_t( new DamageTransaction( ch, full? -ch->damage(): damage ) ) );
		}
		//
	}
}


void RoundsHelper::Stabilize( const bool stabilize )
{
	TransactionGroupHelper helper( gettext("Stabilize selected characters") );
	Combatant::Character::list_t selected = f_charView->getSelected();
    helper.AddTransaction( Transaction::pointer_t( new StabilizeTransaction( selected, stabilize ) ) );
}


void RoundsHelper::RollOther( const std::string& transact_desc, const molib::mo_name_t statId )
{
	auto appSettings(GetAppSettings().lock());
	assert(appSettings);

	Actions::Desensitizer	ad;

	bool make_roll = true;

	if( appSettings->GetDC() )
	{
		const int BUFLEN = 16;
		char buf[BUFLEN+1];
		snprintf( buf, BUFLEN, "%d", appSettings->LastDC() );
        InputWindow dlg( gettext("Enter DC"), *f_parent );
		dlg.Label( gettext("Enter DC:") );
		dlg.Value( buf );
        dlg.set_transient_for( *f_parent );
		const int response_id = dlg.run();
		dlg.hide();
		make_roll = false;
		if( response_id == Gtk::RESPONSE_OK )
		{
			make_roll = true;
			appSettings->LastDC( atoi( dlg.Value().c_str() ) );
		}
	}
	//
	if( make_roll )
	{
		TransactionGroupHelper	helper( transact_desc.c_str() );
		helper.AddTransaction( Transaction::pointer_t(new RollTransaction( statId )) );
		helper.AddTransaction( Transaction::pointer_t(new SortTransaction( statId, appSettings->LastDC() )) );
	}
}


}
//namespace UI

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
