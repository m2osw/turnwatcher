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

// MOLIB
//
#include "mo/mo_base.h"
#include "mo/mo_props.h"

// STL
//
#include <vector>

// LOCAL
//
//#include "config.h"
#include "legacy_character.h"

namespace Application
{


class LegacyApp
{
	public:
		typedef std::shared_ptr<LegacyApp> Pointer;
		typedef std::vector<Pointer>       List;

		LegacyApp();
		virtual ~LegacyApp()	{}

		bool					Load();
		void					GetCharList( Combatant::LegacyCharacter::List& charlist );
		bool					LoadCharacters( molib::moPropBagRef charPropBag );

		bool					inRounds		() const		{ return f_inRounds				; }
		int						roundNumber		() const		{ return f_roundNumber			; }
		int						currentInit		() const		{ return f_currentInit			; }
		int						toolBarPos		() const		{ return f_toolBarPos			; }
		int						window_left		() const		{ return f_windowLeft			; }
		int						window_top		() const		{ return f_windowTop			; }
		int						window_height	() const		{ return f_windowHeight			; }
		int						window_width	() const		{ return f_windowWidth			; }
		int						width			() const		{ return f_windowWidth			; }
		int						height			() const		{ return f_windowHeight			; }
		molib::moWCString		currentFolder	() const		{ return f_currentFolder		; }
		bool					ultraInit		() const		{ return f_ultraInit			; }
		bool					showToolbar		() const		{ return f_showToolbar			; }
		bool					skipDead		() const		{ return f_skipDead				; }

	private:
		bool                             f_inRounds;
		int                              f_roundNumber;
		int                              f_currentInit;
		int                              f_toolBarPos;
		int                              f_windowLeft;
		int                              f_windowTop;
		int                              f_windowHeight;
		int                              f_windowWidth;
		molib::moWCString                f_currentFolder;
		bool                             f_ultraInit;
		bool                             f_showToolbar;
		bool                             f_skipDead;
		Combatant::LegacyCharacter::List f_charList;

		void	Load( molib::moPropBagRef propBag );
};

}
// namespace Application

// vim: ts=4 sw=4 noet syntax=cpp.doxygen

