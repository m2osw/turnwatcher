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

// STL
//
#include <iostream>
#include <string>

// GLIBmm
//
#include <glibmm.h>

// MOLIB
//
#include "mo/mo_base.h"
#include "mo/mo_props.h"

namespace Combatant
{

class LegacyCharacter
{
public:
    	typedef std::shared_ptr<LegacyCharacter> pointer_t;
	typedef std::vector<pointer_t> 	  	 list_t;
	typedef enum { InternalNormal, InternalDelayed, InternalReadied } InternalStatus;

public:
	LegacyCharacter();
	LegacyCharacter( molib::moPropBagRef propBag );
	virtual ~LegacyCharacter()	{}

	void			Load( molib::moPropBagRef propBag );

	Glib::ustring  name()        const { return f_name;        }
	bool           monster()     const { return f_monster;     }
	InternalStatus status()      const { return f_status;      }
	int            maxHP()       const { return f_maxHP;       }
	int            damage()      const { return f_damage;      }
	bool           stabilized()  const { return f_stabilized;  }
	bool           justdropped() const { return f_justdropped; }
	int            init()        const { return f_init;        }
	int            spot()        const { return f_spot;        }
	int            listen()      const { return f_listen;      }
	int            will()        const { return f_will;        }
	int            position()    const { return f_position;    }
	int            initRoll()    const { return f_initRoll;    }
	int            spotRoll()    const { return f_spotRoll;    }
	int            listenRoll()  const { return f_listenRoll;  }
	int            willRoll()    const { return f_willRoll;    }

private:
	Glib::ustring		f_name;		// name of character
	bool			f_monster;	// is this character a monster or pc?
	InternalStatus		f_status;	// normal, delayed or readied action
	int			f_maxHP;	// Maximum hitpoints the character has
	int			f_damage;	// Current damage
	bool			f_stabilized;	// If dying, character may be stabilized
	bool			f_justdropped;	// If true, character just dropped below 0 hitpoints
	int			f_init;		// init modifier
	int			f_spot;		// spot modifier
	int			f_listen;	// listen modifier
	int			f_will;		// will modifier
	int			f_position;	// the real initiative position (1 based, if 0, not sorted)
	int			f_initRoll;	// the actual rolls
	int			f_spotRoll;
	int			f_listenRoll;
	int			f_willRoll;
};


}
// namespace Combatant

// vim: ts=8

