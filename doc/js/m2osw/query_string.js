/////////////////////////////////////////////////////////////////////////////
//
// query_string.js -- retrieve the variables of the URL query string
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
// Usage: find_query_string(name)
//
// name - the name of the variables to get from the query string
//
// This function searches for the specified variable name in the
// list of variables which were given in the URL of the page (also
// called the QUERY_STRING).
//
// If the variable cannot be found, the function returns the string
// 'undefined' (because I.E. does not support the keyword properly)
//



var query_string_names, query_string_values;

query_string_names  = new Array;
query_string_values = new Array;

loc = document.location.search;
if(loc.substring(0, 1) == "?")
{
	// get rid of the question mark
	// (I wonder why they decided to keep it in the first place?!)
	loc = loc.substring(1, loc.length);
	vars = loc.split("&");
	for(v in vars)
	{
		equal = vars[v].indexOf("=");
		query_string_names[v] = vars[v].substring(0, equal);
		query_string_values[v] = vars[v].substring(equal + 1, vars[v].length);
	}
}

function find_query_var(name)
{
	for(v in query_string_names)
	{
		// if name matches, return the corresponding value
		if(query_string_names[v] == name)
		{
			return query_string_values[v];
		}
	}
	return "undefined";
}


//
// Usage: URLDecode(url);
//
// This function converts the %XX definitions to characters.
//
// It also changes the + into spaces.
//
function URLDecode(url)
{
	return decodeURIComponent(url.replace(/\+/g, ' ')).toString();
}


// vim: ts=4
