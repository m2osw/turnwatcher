/////////////////////////////////////////////////////////////////////////////
//
// on_keypress.js -- functions to handle key strokes behaviors
//
// Copyright (C) 2007  Made to Order Software Corp.
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




/** \brief Set the focus on the specified object
 *
 * Function used to accept set the focus on the object
 * specified by its identifier.
 *
 * \param[in] object_id   The button identifier to set the focus on
 */
function set_focus(object_id)
{
	obj = document.getElementById(object_id);
	if(obj) {
		obj.focus();
		obj.select();
	}
}



/** \brief Transform Return/Enter into 'Accept'
 *
 * Function used to accept a Return/Enter keypress and transform it
 * into a submit call.
 *
 * \param[in] evt         The input event
 * \param[in] form_id     The identifier of the form to submit
 */
function submit_on_return(evt, form_id)
{
	// Get the key (depends on the browser)
	key = window.event ? window.event.keyCode : evt.which;

	// Return key?
	if(key == 13) {
		form = document.getElementById(form_id);
		form.submit();
	}
}



// vim: ts=4 sw=4
