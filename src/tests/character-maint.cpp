//===============================================================================
// File:	character_maint.cpp
// Copyright:	Copyright (c) 2005-2007 Made to Order Software, Corp.
//
//		All Rights Reserved.
//
//		This software and its associated documentation contains
//		proprietary, confidential and trade secret information
//		of Made to Order Software, Corp. and except as provided by
//		written agreement with Made to Order Software, Corp.
//
//		a) no part may be disclosed, distributed, reproduced,
//		   transmitted, transcribed, stored in a retrieval system,
//		   adapted or translated in any form or by any means
//		   electronic, mechanical, magnetic, optical, chemical,
//		   manual or otherwise,
//
//		and
//
//		b) the recipient is not entitled to discover through reverse
//		   engineering or reverse compiling or other such techniques
//		   or processes the trade secrets contained therein or in the
//		   documentation.
//
//===============================================================================

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// STL
//
#include <iostream>
#include <cstdlib>

// MOLIB
//
#include <mo_application.h>
#include <mo_image.h>

// GTKMM
// 
#include <gtkmm.h>

// LOCAL
//
#include "common.h"
#include "SingletonManager.h"
#include "transactions.h"


Combatant::CharacterSPtr CreateCharacter( const int id )
{
	const int BUFLEN = 100;
	char BUFFER[BUFLEN+1];
	snprintf( BUFFER, BUFLEN, "Test Character %d", id );
	//
	Combatant::CharacterSPtr ch = new Combatant::Character;
	ch->name   ( BUFFER		);
	ch->monster( true               );
	ch->status ( Combatant::Normal	);
	ch->hitDice( 7			);
	ch->baseHP ( 50			);
	ch->makeAllRolls();
	//
	return ch;
}


void print_current_transaction()
{
	Application::ManagerSPtr		mgr		= Application::Manager::Instance();
	Application::AppSettingsSPtr		appSettings	= mgr->GetAppSettings();
	Transactions::TransactionManagerSPtr	transMgr	= mgr->GetTransactionMgr();

	printf( "Current status message: %s\n", appSettings->StatusMsg().c_str() );
	printf( "Undo top: %s, Redo top: %s\n", transMgr->getUndoListTop().c_str(), transMgr->getRedoListTop().c_str() );
}


void MoveChar( Transactions::TransactionGroupHelper& helper, const Combatant::Characters& chars, const bool up )
{
	printf( "*** Moving character %s %s\n", chars[0]->name().c_str(), up? "up": "down" );
	Transactions::TransactionSPtr tr = new Transactions::MoveSelectedCharacters( chars, up );
	helper.AddTransaction( tr );
	tr->doit();
}



int real_main(int argc, char *argv[])
{
	static const char opts[] =
			"[*];"
			;

	molib::moApplicationSPtr	application = molib::moApplication::Instance();
	application->SetName		( "character-maint"	);
	application->SetVersion		( "1.0"	);
	application->SetCopyright	( "(c) 2007 Made to Order Software Corporation"	);

#ifndef WIN32
	molib::moWCString appPath = molib::moWCString(DATADIR) + "/turnwatcher";
	application->SetApplicationPath( appPath );
#endif
	application->SetOptions(opts, argc, const_cast<const char **>(argv));

	// Setup GTK before creating object. This is required because of the ActionManager
	//
	Gtk::Main	kit( argc, argv );

	// Singleton and managers
	//
	Application::ManagerSPtr		mgr		= Application::Manager::Instance();
	Application::AppSettingsSPtr		appSettings	= mgr->GetAppSettings();
	Attribute::StatManagerSPtr		statMgr		= mgr->GetStatMgr();
	Initiative::InitiativeManagerSPtr	initMgr		= mgr->GetInitMgr();
	Combatant::CharacterManagerSPtr		charMgr		= mgr->GetCharacterMgr();
	Transactions::TransactionManagerSPtr	transMgr	= mgr->GetTransactionMgr();

	transMgr->signal_update().connect( &print_current_transaction );

	assert(appSettings);
	appSettings->UserPath( application->GetUserPath() );
	molib::moWCString confFile = application->GetPrivateUserPath().FilenameChild( DEFAULT_FILENAME );
	appSettings->Load( confFile );

#if 0
	// Insert test chars
	//
	for( int idx = 0; idx < 100; ++idx )
	{
		charMgr->Insert( CreateCharacter( idx ) );
	}
#endif

	statMgr->signal_changed().emit();

	std::cout << "NAME\tINIT\tPOSITION" << std::endl;
	//
	//
	initMgr->SetPositions();
	//
	const Combatant::Characters& chars = initMgr->GetCharacters();
	const int size = static_cast<int>(chars.size());
	const molib::mo_name_t initId = statMgr->initId();
	//
	for( int idx = 0; idx < size; ++idx )
	{
		Combatant::CharacterSPtr ch = chars[idx];
		//
		const int initRoll = ch->getRoll( initId );
		const int position = ch->getPosition();
		//
		std::cout << ch->name().c_str() << "\t\t";
		std::cout << initRoll << "\t";
		std::cout << position << std::endl;
	}

#if 0
	initMgr->Start();
	initMgr->Next( 1, 1 );
	initMgr->Next( 2, 1 );
	initMgr->Next( 4, 1 );
	initMgr->Next( 4, 1 );
	initMgr->Stop();
#else
#if 0
	{
		Transactions::TransactionGroupHelper helper( gettext("Start Rounds") );
		Transactions::TransactionSPtr tr = new Transactions::StartInitTransaction();
		helper.AddTransaction( tr );
	}
	//
	{
		Transactions::TransactionGroupHelper helper( gettext("Next In Initiative Order") );
		helper.AddTransaction( new Transactions::NextInitTransaction );
	}
	//
	{
		Transactions::TransactionGroupHelper helper( gettext("Next In Initiative Order") );
		helper.AddTransaction( new Transactions::NextInitTransaction );
	}
	//
	{
		Transactions::TransactionGroupHelper helper( gettext("Next In Initiative Order") );
		helper.AddTransaction( new Transactions::NextInitTransaction );
	}
	//
	{
		Transactions::TransactionGroupHelper helper( gettext("Next In Initiative Order") );
		helper.AddTransaction( new Transactions::NextInitTransaction );
	}
	//
	{
		Transactions::TransactionGroupHelper	helper( gettext("End Rounds") );
		helper.AddTransaction( new Transactions::StopInitTransaction() );
	}
#endif

	Combatant::Characters sel_chars;
	sel_chars.push_back( chars[0] );

	{
		Transactions::TransactionGroupHelper helper( gettext("Move Character Up 9 times"), false /*auto_doit*/ );
		for( int idx = 0; idx < 100; ++idx )
		{
			//printf( "*** Moving character %s up to pos %d\n",sel_chars[0]->name().c_str(),  idx );
			MoveChar( helper, sel_chars, true /*up*/ );
		}
	}
	//
	{
		Transactions::TransactionGroupHelper helper( gettext("Move Character Down 9 times"), false /*auto_doit*/ );
		for( int idx = 0; idx < 100; ++idx )
		{
			//printf( "*** Moving character %s down to pos %d\n", sel_chars[0]->name().c_str(), idx );
			MoveChar( helper, sel_chars, false /*up*/ );
		}
	}
	//
	{
#if 1
		while( !transMgr->isUndoListEmpty() )
		{
			printf( "*** Undoing transaction %s\n", transMgr->getUndoListTop().c_str() );
			transMgr->undo();
		}
#if 1
		while( !transMgr->isRedoListEmpty() )
		{
			printf( "*** Redoing transaction %s\n", transMgr->getRedoListTop().c_str() );
			transMgr->redo();
		}
#endif
#endif
	}

	appSettings->Save( confFile );

	transMgr->clear();
#endif

	// Cleanup
	//
	mgr->LocalRelease();

	return 0;
}
  

int main(int argc, char *argv[])
{
	std::cout << "Starting test" << std::endl;

	const int r = real_main( argc, argv );

	// we need to tell all the singleton to go away
	molib::moNamePool::Done();
	molib::moImageFileFactory::Done();
	molib::moApplication::Done();

	// print out the objects (moBase derived objects)
	molib::moBase::ShowAllocatedObjects();

	// check all the malloc, realloc, free
	molib::mo_show_allocated_buffers();

	std::cout << "Successful application termination" << std::endl;

	return r;
}

// vim: ts=8 sw=8

