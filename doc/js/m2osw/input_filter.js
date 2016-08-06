/////////////////////////////////////////////////////////////////////////////
//
// input_filter.js -- filter keys in an input tag
// Copyright (C) 2005  Made to Order Software Corp.
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


//
// Usage:
// <input ...
//	onkeypress="javascript:return filter_regex(event, regex);"
//	.../>
//
// event - do not change, that is the exact name you need to use
//	to pass the event down
// regex - a string that represents a regular expression to be
//	matched for the key to be accepted
//
// This function matches the new key with the regular expression.
// If the key does not match the regular expression, then it is
// dropped (i.e. not used in input field)
//
function filter_regex(evt, regex, noreturn)
{
	var key = window.event ? window.event.keyCode : evt.which;
	if(key == 13 && noreturn) {
		return false;
	}
	return key < 32 || key == 127 || regex.test(String.fromCharCode(key));
}

