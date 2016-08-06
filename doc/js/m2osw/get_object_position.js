/////////////////////////////////////////////////////////////////////////////
//
// get_object_position.js -- retrieve the position of an HTML object
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
// Note a good documentation in regard to "screen" (really
// the window) sizes is here:
//
//		http://www.howtocreate.co.uk/tutorials/javascript/browserwindow
//
// The one thing which it doesn't mention is that in Internet Explorer
// the sizes are not known unless you are in the BODY.
//
var screen_width = 0, screen_height = 0;

function get_screen_size()
{
		if(window.innerWidth) {
			screen_width  = window.innerWidth;
			screen_height = window.innerHeight;
		}
		else if(document.documentElement && document.documentElement.clientWidth) {
			screen_width  = document.documentElement.clientWidth;
			screen_height = document.documentElement.clientHeight;
		}
		else if(document.body) {
			screen_width  = document.body.clientWidth;
			screen_height = document.body.clientHeight;
		}
		else {
			// Well, really we don't know... but we better have some good default.
			screen_width  = 1270;
			screen_height = 1014;
		}
}


//
// Usage: get_x(name, placement)
//
// name - the name of the object you want the x coordinate of
// placement - the placement relative to the object
// 	placement can be 'left', 'center' or 'right'
//
// This function searches for the object defined by name and
// compute its horizontal position in the screen.
//
// If the object cannot be found, the function returns 0.
//
function get_x(name, placement)
{
	// make sure the named object exists
	var o = document.getElementById(name);
	if(o == undefined) {
		// This happens when you go to a new page but the
		// system still tries to process some event.
		return 0;
	}

	return get_object_x(o, placement);
}

//
// Usage: get_object_x(o, placement)
//
// o - the object you want the x coordinate of
// placement - the placement relative to the object
// 	placement can be 'left', 'center' or 'right'
//
// This function computes the current horizontal position
// of the object.
//
function get_object_x(o, placement)
{
	if(o == undefined) {
		// This happens when you go to a new page but the
		// system still tries to process some event.
		return 0;
	}

	// get the position and adjust it with the placement
	var x = o.offsetLeft;
	if(placement == 'right') {
		x += o.offsetWidth;
	}
	else if(placement == 'center') {
		x += o.offsetWidth / 2;
	}

	// compute the position within the parent recursively
	p = o.offsetParent;
	while(p != undefined) {
		x += p.offsetLeft;
		p = p.offsetParent;
	}

	return x;
}

//
// Usage: get_width(name)
//
// name - the name of the object you want the width of
//
// This function searches for the object defined by name and
// compute its width.
//
// If the object can't be found, the function returns 0.
//
function get_width(name)
{
	// make sure the named object exists
	var o = document.getElementById(name);
	if(o == undefined) {
		// This happens when you go to a new page but the
		// system still tries to process some event.
		return 0;
	}

	// return the height
	return o.offsetWidth;
}

//
// Usage: get_object_width(o)
//
// o - the object you want the width of
//
// This function retrieves the width of the specified object.
//
// If the object is not defined, then the function returns 0.
//
function get_object_width(o)
{
	if(o == undefined) {
		// This happens when you go to a new page but the
		// system still tries to process some event.
		return 0;
	}

	// return the height
	return o.offsetWidth;
}



//
// Usage: get_y(name, placement)
//
// name - the name of the object you want the y coordinate of
// placement - the placement relative to the object
// 	placement can be 'top', 'middle' or 'bottom'
// 	(note that 'left', 'center' and 'right' work too but
// 	that's just to make sure that it works in more cases)
//
// This function searches for the object defined by name and
// compute its vertical position in the screen.
//
// If the object can't be found, the function returns 0.
//
function get_y(name, placement)
{
	// make sure the named object exists
	var o = document.getElementById(name);
	if(o == undefined) {
		// This happens when you go to a new page but the
		// system still tries to process some event.
		return 0;
	}

	return get_object_y(o, placement);
}

//
// Usage: get_object_y(o, placement)
//
// o - the object you want the y coordinate of
// placement - the placement relative to the object
// 	placement can be 'top', 'middle' or 'bottom'
// 	(note that 'left', 'center' and 'right' work too but
// 	that's just to make sure that it works in more cases)
//
// This function computes the position of the specified
// object.
//
function get_object_y(o, placement)
{
	if(o == undefined) {
		// This happens when you go to a new page but the
		// system still tries to process some event.
		return 0;
	}

	// get the position and adjust it with the placement
	var y = o.offsetTop;
	if(placement == 'bottom' || placement == 'right') {
		y += o.offsetHeight;
	}
	else if(placement == 'center' || placement == 'middle') {
		y += o.offsetHeight / 2;
	}

	// compute the position within the parent recursively
	// NOTE: somehow, trying to use p.scrollTop has no effect
	p = o.offsetParent;
	while(p != undefined) {
		y += p.offsetTop;
		p = p.offsetParent;
	}

	return y;
}

//
// Usage: get_height(name)
//
// name - the name of the object you want the height of
//
// This function searches for the object defined by name and
// compute its height.
//
// If the object can't be found, the function returns 0.
//
function get_height(name)
{
	// make sure the named object exists
	var o = document.getElementById(name);
	if(o == undefined) {
		// This happens when you go to a new page but the
		// system still tries to process some event.
		return 0;
	}

	// return the height
	return o.offsetHeight;
}

//
// Usage: get_height(name)
//
// o - the object you want the height of
//
// This function retrieves the height of the specified object.
//
// If the object is not defined, the function returns 0.
//
function get_object_height(o)
{
	if(o == undefined) {
		// This happens when you go to a new page but the
		// system still tries to process some event.
		return 0;
	}

	// return the height
	return o.offsetHeight;
}


//
// Usage: positionObject(name, x, y)
//
// name - the name of the object you want to position
// x - the horizontal position
// y - the vertical position
//
// This function searches for the object defined by name and
// sets its left and top position.
//
// If the object can't be found, the function does nothing.
//
function set_object_position(name, x, y)
{
	// make sure the named object exists
	var o = document.getElementById(name);
	if(o == undefined) {
		// This happens when you go to a new page but the
		// system still tries to process some event.
		return 0;
	}

	o.style.left = x;
	o.style.top = y;
}

//
// Usage: set_object_width(o, w)
//
// o - the object you want to change the width
// w - the new width of the object
//
// This function sets the width of the object to the specified value.
//
// When w is all digits, it is assumed to be in pixels. You can use
// one of the following suffices and strings:
//
// . <width>px (pixels)
// . <width>em
// . auto
//
//
// If the object is not defined, then the function does nothing.
//
function set_object_width(o, w)
{
	if(o == undefined) {
		return 0;
	}

	if(/^[0-9]+$/.exec(w)) {
		o.style.width = w + 'px';
	}
	else {
		o.style.width = w;
	}
}


//
// Usage: set_object_height(o, h)
//
// o - the object you want to change the height
// h - the new height of the object
//
// This function sets the height of the object to the specified value.
//
// When h is all digits, it is assumed to be in pixels. You can use
// one of the following suffices and strings:
//
// . <height>px (pixels)
// . <height>em
// . auto
//
// If the object is not defined, then the function does nothing.
//
function set_object_height(o, h)
{
	if(o == undefined) {
		return 0;
	}

	if(/^[0-9]+$/.exec(h)) {
		o.style.height = h + 'px';
	}
	else {
		o.style.height = h;
	}
}


// vim: ts=4 sw=4
