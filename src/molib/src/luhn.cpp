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
#pragma implementation "mo/mo_luhn.h"
#endif

#include	"mo/mo_luhn.h"


namespace molib
{
namespace luhn
{


/** \brief Check the validity of a credit card number.
 *
 * This function checks whether a number is a valid credit card
 * number.
 *
 * Note that many numbers are valid credit card numbers such as:
 *
 * 4433221111223344
 *
 * This is obviously not a valid Visa account however.
 *
 * Yet, it is easy to make a mistake whenever you enter your
 * credit card number. If that happens, you can catch the
 * mistake early by using this test function.
 *
 * \return true when the card number is valid.
 *
 * \sa moSum()
 */
bool moIsValid(const moWCString& number)
{
	int	sum;

	// get the sum
	sum = moSum(number);
	if(sum < 0) {
		// invalid string
		return false;
	}

	// must be a multiple of 10
	return sum % 10 == 0;
}


/** \brief Compute the Luhn number.
 *
 * This function computes the Luhn number of a credit card number.
 *
 * The result must be a multiple of 10 to be considered a valid credit
 * card number (i.e. moSum(...) % 10 == 0).
 *
 * If you do not need the sum, then use moIsValid() instead.
 *
 * The input string can include spaces and digits only and must be
 * at least 13 digits and at most 16 digits. Spaces are ignored.
 *
 * The function returns an error if something is wrong in the
 * input \p number parameter. One of the following will be
 * returned:
 *
 * \li ERROR_TOO_LONG
 * \li ERROR_TOO_SHORT
 * \li ERROR_NOT_A_DIGIT
 *
 * \param[in] number The credit card number to check.
 *
 * \return The sum or an error.
 *
 * \sa moIsValid()
 */
int moSum(const moWCString& number)
{
	int		l, p, sum, c;
	const char	*s;
	bool		d;

	s = number.c_str();
	p = static_cast<int>(strlen(s));
	sum = 0;
	l = 0;
	d = false;
	while(p > 0) {
		--p;
		c = s[p];
		if(c == ' ') {
			continue;
		}
		if(c < '0' || c > '9') {
			return ERROR_NOT_A_DIGIT;
		}
		if(l >= 16) {
			return ERROR_TOO_LONG;
		}
		++l;
		c -= '0';
		if(d) {
			c *= 2;
			if(c > 9) {
				c -= 9;
			}
		}
		d = !d;
		sum += c;
	}
	if(l < 13) {
		return ERROR_TOO_SHORT;
	}

	return sum;
}

};	// namespace luhn
};	// namespace molib

