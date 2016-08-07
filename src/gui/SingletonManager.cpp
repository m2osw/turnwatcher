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




#include "SingletonManager.h"

namespace Application
{

Manager::SharedPointer Manager::f_instance;

Manager::Pointer Manager::Instance()
{
	if( !f_instance )
	{
		f_instance.reset( new Manager );
	}

	return f_instance;
}


void Manager::LocalRelease()
{
	f_instance.reset();
}


Manager::Manager()
{
	ResetActionsMgr();
	f_appSettings    .reset( new Application::AppSettings         );
	f_characterMgr   .reset( new Combatant::CharacterManager      );
	f_initMgr        .reset( new Initiative::InitiativeManager    );
	f_statMgr        .reset( new Attribute::StatManager           );
	f_transactionMgr .reset( new Transactions::TransactionManager );
}


void Manager::ResetActionsMgr()
{
	f_actionsMgr .reset( new Actions::Manager );
}


#if 0
void Manager::Load( const molib::moWCString& fileName )
{
    std::ifstream ifs( fileName.c_str() );
    assert(ifs.good());
    boost::archive::xml_iarchive ia(ifs);
    ia >> BOOST_SERIALIZATION_NVP(*this);
}


void Manager::Save( const molib::moWCString& fileName ) const
{
    std::ofstream ofs( fileName.c_str() );
    assert(ofs.good());
    boost::archive::xml_oarchive oa(ofs);
    oa << BOOST_SERIALIZATION_NVP(*this);
}
#endif


#if 0 //defined( DEMO_VERSION )

void Manager::LoadDemoCharacters()
{
	StatManager::Pointer statMgr = Application::Manager::Instance()->GetStatMgr();
	const molib::mo_name_t initId   = statMgr.lock()->initId();
	const molib::mo_name_t spotId   = statMgr.lock()->spotId();
	const molib::mo_name_t listenId = statMgr.lock()->listenId();
	const molib::mo_name_t willId   = statMgr.lock()->willId();
	const molib::mo_name_t levelId  = statMgr.lock()->levelId();

	Combatant::Character::Pointer ch = new Combatant::Character();
	ch->name( "Errol" );
	ch->monster( false );
	ch->baseHP( 36 );
	ch->setMod( initId, 4 );
	ch->setMod( spotId, 2 );
	ch->setMod( listenId, 1 );
	ch->setMod( willId, 4 );
	ch->setMod( levelId, 5 );
	AddCharacter( ch, NULL );
	//
	ch = new Combatant::Character();
	ch->name( "Stubs" );
	ch->monster( false );
	ch->baseHP( 45 );
	ch->setMod( initId, 1 );
	ch->setMod( spotId, -1 );
	ch->setMod( listenId, -1 );
	ch->setMod( willId, 5 );
	ch->setMod( levelId, 5 );
#if 0
	Effects::EffectSPtr effect = new Effects::Effect;
	effect->name( "Bull Strength" );
	effect->description( "Gives a +4 to strength" );
	effect->totalRounds( 110 );
	ch->addEffect( effect );
#endif
	AddCharacter( ch, NULL );
	//
	ch = new Combatant::Character();
	ch->name( "Arial" );
	ch->monster( false );
	ch->baseHP( 24 );
	ch->setMod( initId, 1 );
	ch->setMod( spotId, 1 );
	ch->setMod( listenId, 1 );
	ch->setMod( willId, 4 );
	ch->setMod( levelId, 5 );
	AddCharacter( ch, NULL );
	//
	ch = new Combatant::Character();
	ch->name( "K the Knife" );
	ch->monster( false );
	ch->baseHP( 37 );
	ch->setMod( initId, 4 );
	ch->setMod( spotId, 4 );
	ch->setMod( listenId, 4 );
	ch->setMod( willId, 6 );
	ch->setMod( levelId, 5 );
	AddCharacter( ch, NULL );
	//
	ch = new Combatant::Character();
	ch->name( "Sinclair" );
	ch->monster( true );
	ch->baseHP( 35 );
	ch->setMod( initId, 4 );
	ch->setMod( spotId, 2 );
	ch->setMod( listenId, 2 );
	ch->setMod( willId, 8 );
	ch->setMod( levelId, 5 );
	AddCharacter( ch, NULL );
	//
	ch = new Combatant::Character();
	ch->name( "Dench" );
	ch->monster( true );
	ch->baseHP( 90 );
	ch->setMod( initId, 3 );
	ch->setMod( spotId, 1 );
	ch->setMod( listenId, 1 );
	ch->setMod( willId, 6 );
	ch->setMod( levelId, 5 );
	AddCharacter( ch, NULL );
}

#endif // DEMO_VERSION


}
// namespace Application


