/////////////////////////////////////////////////////////////////////////////
//
// insert_text.js -- insert text in an input at the current cursor position
// Copyright (C) 2005-2008  Made to Order Software Corp.
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


/** \brief Insert text in a field.
 *
 * Usually, you will call this function from a link or a button
 * which is to insert the to_insert string in the specified
 * field.
 *
 * The field is expected to be an <input ...> of type text or
 * a <textarea>. to_insert can be any valid javascript string.
 *
 * The function does not return anything useful.
 *
 * \param[in] field       The field where the text is inserted
 * \param[in] to_insert   A string with the text to be inserted
 */
function insert_text(field, to_insert)
{
	if(document.selection) {
		// IE support
		field.focus();
		var sel = document.selection.createRange();
		sel.text = to_insert;
		field.focus();
	}
	else if(field.selectionStart || field.selectionStart == '0') {
		// MOZILLA/NETSCAPE/FIREFOX support
		var start_pos = field.selectionStart;
		var end_pos = field.selectionEnd;
		var cursor_pos = start_pos + to_insert.length;
		field.value = field.value.substring(0, start_pos)
						+ to_insert
						+ field.value.substring(end_pos, field.value.length);
		field.selectionStart = cursor_pos;
		field.selectionEnd = cursor_pos;
	}
	else {
		// well... use an ugly fallback
		field.value += to_insert;
	}
}


