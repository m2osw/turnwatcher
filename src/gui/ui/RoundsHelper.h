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

#include "CharacterView.h"
#include "HUDWindow.h"
#include "ManagerBase.h"
#include "transaction.h"

#include "mo/mo_name.h"

#include <memory>
#include <string>

namespace UI
{


class RoundsHelper : private Application::ManagerBase
{
public:
	typedef std::shared_ptr<RoundsHelper> pointer_t;

	RoundsHelper	( Gtk::Window*   parent
					, CharacterView* charView
					, HUDWindow*     hudView
					);
	~RoundsHelper();

	void Start();
	void End();
	void Next();
	void Delay();
	void Ready();
	void JumpIn();
	void Damage();
	void MoveCharacters( const bool up );
	void Stabilize( const bool stabilize );
	void RollInitiative();
	void RollOther( const std::string& transact_desc, const molib::mo_name_t statId );

private:
    Gtk::Window*	f_parent;
    CharacterView*	f_charView;
    HUDWindow*		f_hudView;

	bool CreateNextInIntiativeTransaction( Transactions::TransactionGroup::pointer_t group );
	void DisplayPendingEffects();
	bool RollInitiative( Transactions::TransactionGroup::pointer_t group, bool allowCancel );
};

}
// namespace UI

// vim: ts=4 sw=4 noexpandtab syntax=cpp.doxygen
