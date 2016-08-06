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





#ifndef MO_RANDOM_H
#define	MO_RANDOM_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_BASE_H
#include	"mo_base.h"
#endif


namespace molib
{

class MO_DLL_EXPORT moRandomBase : public moBase
{
public:
	//			moRandomBase(void);
	//virtual		~moRandomBase();

	virtual void		SetSeed(const void *seed) = 0;
	virtual void		GetSeed(void *seed) const = 0;
	virtual unsigned long	Random(unsigned int count = 0) = 0;
};

typedef moSmartPtr<moRandomBase>		moRandomBaseSPtr;





class MO_DLL_EXPORT moRandom : public moRandomBase
{
public:
	// the following defines the number of long words of seed
	static const unsigned int MO_RANDOM_SEED_N = 25;
	static const unsigned int MO_RANDOM_SEED_M = 7;

				moRandom(void);
	virtual			~moRandom();

	virtual void		SetSeed(const void *seed);
	virtual void		GetSeed(void *seed) const;
	virtual unsigned long	Random(unsigned int count = 0);

private:
	unsigned long		f_seed[MO_RANDOM_SEED_N];
	unsigned long		f_k;
};

typedef moSmartPtr<moRandom>		moRandomSPtr;




};			// namespace molib;

// vim: ts=8 sw=8
#endif	// #ifndef MO_RANDOM_H

