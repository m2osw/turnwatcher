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

#include "base/ActionsManager.h"
#include "base/AppSettings.h"
#include "base/CharacterManager.h"
#include "base/CharacterModel.h"
#include "base/InitiativeManager.h"
#include "base/StatManager.h"
#include "base/transaction.h"

// molib
//
#include "mo/mo_string.h"

// stl
//
#include <memory>

namespace Application
{

class ManagerBase
{
protected:
	ManagerBase() {}

    std::weak_ptr<Actions::Manager>                 GetActionsMgr     () const { return Actions::Manager::Instance                 (); }
    std::weak_ptr<Application::AppSettings>         GetAppSettings    () const { return Application::AppSettings::Instance         (); }
    std::weak_ptr<Combatant::CharacterManager>      GetCharacterMgr   () const { return Combatant::CharacterManager::Instance      (); }
    std::weak_ptr<Combatant::CharacterModel>        GetCharacterModel () const { return Combatant::CharacterModel::Instance        (); }
    std::weak_ptr<Initiative::InitiativeManager>    GetInitMgr        () const { return Initiative::InitiativeManager::Instance    (); }
    std::weak_ptr<Attribute::StatManager>           GetStatMgr        () const { return Attribute::StatManager::Instance           (); }
    std::weak_ptr<Transactions::TransactionManager> GetTransactionMgr () const { return Transactions::TransactionManager::Instance (); }
};


}
// namespace Application

// vim: ts=4 sw=4 noet syntax=cpp.doxygen

