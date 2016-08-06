/////////////////////////////////////////////////////////////////////////////
//
// cookies.js -- retrieve the variables from the cookies string
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
// Usage: read_cookie_vars()
//
// This function needs to be called to initialize the cookie arrays.
// It reads the string of cookies and transform it in an array of
// names and an array of values.
//
// If your code changes a cookie, you should call this function again
// to make sure that the arrays are up to date.
//
//
// Usage: find_cookie_var(name)
//
// name - the name of the variables to get from the cookies
//
// This function searches for the specified variable name in the
// list of variables which were found in the cookies of the document.
//
// If the variable cannot be found, the function returns the string
// 'undefined' (because I.E. does not support the keyword properly)
//



var cookie_string_names, cookie_string_values;

cookie_string_names  = new Array;
cookie_string_values = new Array;

function read_cookie_vars()
{
	ck = document.cookie;
	vars = ck.split(";");
	for(v in vars)
	{
		equal = vars[v].indexOf("=");
		if(vars[v].substring(0, 1) == " ")
		{
			cookie_string_names[v] = vars[v].substring(1, equal);
		}
		else
		{
			cookie_string_names[v] = vars[v].substring(0, equal);
		}
		cookie_string_values[v] = vars[v].substring(equal + 1, vars[v].length);
	}
}
read_cookie_vars();

function find_cookie_var(name)
{
	for(v in cookie_string_names)
	{
		if(cookie_string_names[v] == name)
		{
			return cookie_string_values[v];
		}
	}
	return "undefined";
}


// vim: ts=4
