/////////////////////////////////////////////////////////////////////////////
//
// set_class.js -- set the class in an element
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
// Usage: set_class(element, name)
//
// Assign the specified class (name) to the element.
//




function set_class(element, name)
{
	element.setAttribute('class', name);
	element.setAttribute('className', name);	// This is for IE
}


function get_class(element)
{
	name = element.getAttribute('className');
	if(name.length == 0) {
		name = element.getAttribute('class');
	}

	return name;
}

