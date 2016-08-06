/////////////////////////////////////////////////////////////////////////////
//
// set_text.js -- set text in an element
// Copyright (C) 2005-2007  Made to Order Software Corp.
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
// Usage: set_text(element, text)
//
// Sets the specified text in the element.
//




function set_text(element, text)
{
	if(element) {
		if(element.textContent) {
			// MOZILLA/NETSCAPE/FIREFOX support
			element.textContent = text;
		}
		else {
			// IE, Opera support
			// Do we need to unescape the text?
			element.innerHTML = text;
		}
	}
}


function get_text(element)
{
	if(!element) {
		return '';
	}
	if(element.textContent) {
		// MOZILLA/NETSCAPE/FIREFOX support
		return element.textContent;
	}
	else {
		// IE, Konqueror, Opera support
		// Do we need to unescape the text?
		return element.innerHTML;
	}
}

