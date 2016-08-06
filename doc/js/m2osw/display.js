/////////////////////////////////////////////////////////////////////////////
//
// display.js -- hide and show objects as required
// Copyright (C) 2007-2008  Made to Order Software Corp.
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

/** \brief make the named box visible
 *
 * This function changes the display style from 'none' to the specified
 * mode. The new display type needs to correspond to the tag being changed.
 * This is one of:
 *
 *		auto				*			the function tries to figure it out
 *		block				DIV,P		shown as a block
 *		inline				A,INPUT		shown inlined (normal flow)
 *		inline-block		?			shown inlined, what's inside is a block
 *		inline-table		TABLE		table rectangle shown inlined
 *		list-item			LI			show as a list item
 *		none				*			hidding
 *		run-in				?			show inlined or as a block
 *		table				TABLE		table rectangle shown as a block
 *		table-caption		CAPTION		this is the caption of a table
 *		table-cell			TD, TH		this is a cell
 *		table-column		COL			this is a group of cells representing a column
 *		table-column-group	COLGROUP	this is a group of columns
 *		table-footer-group	TFOOT		this is a group representing the footer of a table
 *		table-header-group	THEAD		this is a group representing the header of a table
 *		table-row			TR			this is a row of cells
 *		table-row-group		TBODY		this is a group of rows
 *
 * \note
 * If 'auto' is used and the function does not know the tag, it generates
 * and alert() and then uses 'inline'. Feel free to fix the function or
 * remove the alert.
 *
 * \param[in] id     The identifier of the box to show
 * \param[in] mode   This represents the type of what needs to be hidden;
 *                   if unspecified, use 'auto'
 */
function show_box(id, mode)
{
	// At times, we call this function too soon or too late
	// and then the box is not available, avoid problems by
	// using a condition. You will know whether it works or
	// not since you will test your site.
	var box = document.getElementById(id);
	if(box) {
		if(!mode || mode == 'auto') {
			switch(box.tagName) {
			case 'A': case 'a':
			case 'INPUT': case 'input':
			case 'FORM': case 'form':
				mode = 'inline';
				break;

			case 'DIV': case 'div':
			case 'P': case 'p':
				mode = 'block';
				break;

			case 'TABLE': case 'table':
				// IE6 and earlier do not support 'table' mode
				if(is_IE) {
					mode = 'inline';
				}
				else {
					mode = 'table';
				}
				break;

			case 'TBODY': case 'tbody':
				// IE6 and earlier do not support 'table' mode
				if(is_IE) {
					mode = 'inline';
				}
				else {
					mode = 'table-row-group';
				}
				break;

			case 'TR': case 'tr':
				// IE6 and earlier do not support 'table' mode
				if(is_IE) {
					mode = 'inline';
				}
				else {
					mode = 'table-row';
				}
				break;

			case 'TD': case 'td':
				// IE6 and earlier do not support 'table' mode
				if(is_IE) {
					mode = 'inline';
				}
				else {
					mode = 'table-cell';
				}
				break;

			default:
				alert('Unsupported tag type in show_box(): "' + box.tagName + '".');
				mode = 'inline';
				break;

			}
		}
		box.style.display = mode;
	}
}




/** \brief hide the named box
 *
 * This function marks the named box as hidden.
 *
 * More precisely, it sets the display style to 'none' which
 * is done by calling show_box() with 'none' for the mode.
 *
 * \param[in] id   The identifier of the box to hide
 */
function hide_box(id)
{
	show_box(id, 'none');
}



// vim: ts=4 sw=4
