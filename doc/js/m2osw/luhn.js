/////////////////////////////////////////////////////////////////////////////
//
// luhn.js -- check a credit card number using the luhn check
// Copyright (C) 2008  Made to Order Software Corp.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
/////////////////////////////////////////////////////////////////////////////
//
// You can find a complete copy of the GPL at the following address:
//
//	http://gpl.m2osw.com
//
// Also, you can find original copies at the GNU foundation:
//
//	http://www.gnu.org
//


/** \brief Check a credit card number with the Luhn check.
 *
 * This function verifies that the credit card number is
 * acceptable as per the Luhn check.
 *
 * This does not mean the card is otherwise valid, but at
 * least we avoid accepting totally invalid cards (especially
 * if the person simply missed a digit or something of the
 * sort.)
 *
 * This early test can be run when your customer clicks
 * on the "Send Payment" button to avoid sending but valid
 * cards.
 *
 * \param[in] n  The credit card number to check
 *
 * \return true whenever the credit card is considered valid
 */
function luhn(n)
{
	var sum = 0;
	var d = false;
	var p = n.length;
	var l = 0;
	while(p > 0) {
		--p;
		var c = n[p];
		if(c != ' ') {
			if(l >= 16) { // too long
				return false;
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
	}
	if(l < 13) { // too short
		return false;
	}

	// if multiple of 10, it is valid
	return sum % 10 == 0;
}

