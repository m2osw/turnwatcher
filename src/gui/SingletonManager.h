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

#include "ActionsManager.h"
#include "AppSettings.h"
#include "CharacterManager.h"
#include "InitiativeManager.h"
#include "StatManager.h"
#include "transaction.h"

// molib
//
#include "mo/mo_string.h"

// stl
//
#include <memory>

namespace Application
{

class Manager
{
public:
	typedef std::weak_ptr<Manager> 			    Pointer;

	static Pointer								Instance();
	static void									LocalRelease();

	//void Load( const molib::moWCString& filename );
	//void Save( const molib::moWCString& filename ) const;

#if 0 //defined( DEMO_VERSION )
	void LoadDemoCharacters();
#endif

	std::weak_ptr<Actions::Manager>					GetActionsMgr()		const { return f_actionsMgr		; }
	void											ResetActionsMgr();
	//
	std::weak_ptr<Application::AppSettings>			GetAppSettings()  	const { return f_appSettings	; }
	std::weak_ptr<Combatant::CharacterManager>		GetCharacterMgr() 	const { return f_characterMgr	; }
	std::weak_ptr<Initiative::InitiativeManager>	GetInitMgr()      	const { return f_initMgr		; }
	std::weak_ptr<Attribute::StatManager>			GetStatMgr()      	const { return f_statMgr		; }
	std::weak_ptr<Transactions::TransactionManager>	GetTransactionMgr()	const { return f_transactionMgr	; }

private:
	typedef std::shared_ptr<Manager>  			SharedPointer;
	static SharedPointer						f_instance;
	//
	Actions::Manager::Pointer                 	f_actionsMgr;
	Application::AppSettings::Pointer         	f_appSettings;
	Combatant::CharacterManager::Pointer      	f_characterMgr;
	Initiative::InitiativeManager::Pointer    	f_initMgr;
	Attribute::StatManager::Pointer           	f_statMgr;
	Transactions::TransactionManager::Pointer 	f_transactionMgr;

												Manager();
												Manager( const Manager& )		{}
	Manager&									operator =( const Manager& )	{return *this;}
};


class ManagerBase
{
protected:
	ManagerBase() : f_mgr( Manager::Instance() ) {}

    std::weak_ptr<Actions::Manager>                 GetActionsMgr()     const { return f_mgr.lock()->GetActionsMgr     (); }
	std::weak_ptr<Application::AppSettings>         GetAppSettings()    const { return f_mgr.lock()->GetAppSettings    (); }
	std::weak_ptr<Combatant::CharacterManager>      GetCharacterMgr()   const { return f_mgr.lock()->GetCharacterMgr   (); }
	std::weak_ptr<Initiative::InitiativeManager>    GetInitMgr()        const { return f_mgr.lock()->GetInitMgr        (); }
	std::weak_ptr<Attribute::StatManager>           GetStatMgr()        const { return f_mgr.lock()->GetStatMgr        (); }
	std::weak_ptr<Transactions::TransactionManager> GetTransactionMgr() const { return f_mgr.lock()->GetTransactionMgr (); }

	Manager::Pointer GetSingletonMgr() const { return f_mgr; }

private:
	Manager::Pointer f_mgr;

};

}
// namespace Application

// vim: ts=4 sw=4 noet syntax=cpp.doxygen

