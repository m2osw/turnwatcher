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





#ifdef MO_PRAGMA_INTERFACE
#pragma implementation "mo/mo_random.h"
#endif

#include	"mo/mo_random.h"

namespace molib
{


/************************************************************ DOC:

CLASS

	moRandom

NAME

	Constuctor - initialize a random object

SYNOPSIS

	moRandom(void);

DESCRIPTION

	This function initialize a random object so it can be used
	right away.

	These random values are only pseudo random, not pure random
	values.

SEE ALSO

	Destructor

*/
moRandom::moRandom(void)
{
	static unsigned long	start_values[MO_RANDOM_SEED_N] = {
		/* default initial 25 seeds */
		0x95F24DAB, 0x0B685215, 0xE76CCAE7, 0xAF3EC239, 0x715FAD23,
		0x24A590AD, 0x69E4B5EF, 0xBF456141, 0x96BC1B7B, 0xA7BDF825,
		0xC1DE75B7, 0x8858A9C9, 0x2DA87693, 0xB657F9DD, 0xFFDC8A9F,
		0x8121DA71, 0x8B823ECB, 0x885D05F5, 0x4E20CD47, 0x5A9AD5D9,
		0x512C0C03, 0xEA857CCD, 0x4CC1D30F, 0x8891A8A1, 0xA6B7AADB
	};

	memcpy(f_seed, start_values, sizeof(f_seed));		/* Flawfinder: ignore */
	f_k = 0;
}




/************************************************************ DOC:

CLASS

	moRandom

NAME

	Destructor - clean up a random object

SYNOPSIS

	~moRandom();

DESCRIPTION

	This function cleans up a random object.

SEE ALSO

	Constructor

*/
moRandom::~moRandom()
{
}



/************************************************************ DOC:

CLASS

	moRandom

NAME

	SetSeed - set seeds to user defined values

SYNOPSIS

	void SetSeed(const void *seed);

PARAMETERS

	seed - a pointer to MO_RANDOM_SEED_N unsigned longs

DESCRIPTION

	This function can be used to modify the seeds to a
	set of values which are determined by the caller.

	The pointer is expected to be of the proper size
	(i.e. MO_RANDOM_SEED_N).

	This function has the side effect of reseting the
	seed index.

NOTES

	The first MO_RANDOM_SEED_N values are equal to the
	seed. Only the following values will be a mixed
	set of the original seed value. In order to avoid
	reading the same values as the seeds, it is advice
	to use the Random() call with a count value of
	one hunder or more.

SEE ALSO

	GetSeed

*/
void moRandom::SetSeed(const void *seed)
{
	memcpy(f_seed, seed, sizeof(f_seed));		/* Flawfinder: ignore */
	f_k = 0;
}




/************************************************************ DOC:

CLASS

	moRandom

NAME

	GetSeed - copy current seeds in a user buffer

SYNOPSIS

	void GetSeed(void *seed) const;

PARAMETERS

	seed - a pointer to MO_RANDOM_SEED_N unsigned longs

DESCRIPTION

	This function copies the current seeds from the random
	object to the user specified buffer. The user buffer
	data are lost.

	Note that the seed pointer is expected to be large
	enough to support all the seeds (i.e. MO_RANDOM_SEED_N
	unsigned longs).

SEE ALSO

	SetSeed

*/
void moRandom::GetSeed(void *seed) const
{
	memcpy(seed, f_seed, sizeof(f_seed));		/* Flawfinder: ignore */
}




/************************************************************ DOC:

CLASS

	moRandom

NAME

	Random - generates the next random value

SYNOPSIS

	unsigned long Random(unsigned int count = 0);

PARAMETERS

	count - skip count values first

DESCRIPTION

	This function computes the next value and returns it. Note
	that the value of 'count' can be set to any number of values
	to be skipped before to return the next long in order to
	make the value even more random.

	It is important to understand that it is better to use the
	higher bits of the returned value rather than the lower ones.
	These are more randomized than the lower bits. Thus, a function
	which uses this Random() function to use 8 bits should use
	only the top 8 bits by shifting the result by 24 as following:

		my_byte = Random() >> 24;

NOTES

	This function isn't a constant function because it modifies
	the current state of the random seeds.

	The use of the 'count' parameter is to be avoided if you want
	the function to really return values which are uniformly
	distributed.

SEE ALSO

	SetSeed, GetSeed

COPYRIGHT

	This function is fully based on some work by M. Matsumoto
	in 1994 & 1996. (email: matumoto@math.keio.ac.jp) It comes
	from his source named tt800.c. Special thanks.

*/
unsigned long moRandom::Random(unsigned int count)
{
	unsigned long	y, i, j;

	if(count > 10000) {
		count = 10000;
	}

	count += f_k;
	while(count >= MO_RANDOM_SEED_N) {
		i = 0;
		j = MO_RANDOM_SEED_M;
		while(i < MO_RANDOM_SEED_N) {
			y = f_seed[i];				// below is the a vector
			f_seed[i] = f_seed[j] ^ (y >> 1) ^ (y & 1 ? 0x8EBFD028 : 0);
			i++;
			j++;
			if(j == MO_RANDOM_SEED_N) {
				j = 0;
			}
		}
		count -= MO_RANDOM_SEED_N;
	}
	f_k = count;

	y = f_seed[f_k];
	y ^= (y <<  7) & 0x2B5B2500;	// s and b magic vectors
	y ^= (y << 15) & 0xDB8B0000;	// t and c magic vectors
	y ^= y >> 16;			// lower bits look better this way

	f_k++;

	return y;
}






};			// namespace molib;

// vim: ts=8

