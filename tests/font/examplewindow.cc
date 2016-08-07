//$Id: examplewindow.cc,v 1.3 2004/02/10 14:49:00 murrayc Exp $ -*- c++ -*-

/* gtkmm example Copyright (C) 2002 gtkmm development team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "examplewindow.h"
#include <iostream>


ExampleWindow::ExampleWindow()
	: m_Button("Sans 12"),
	m_fixedLabel("Fixed example text")
{
	set_title("Gtk::FontSelectionDialog example");

	m_vBox.pack_start( m_Button );
	m_vBox.pack_start( m_fixedLabel );
	add(m_vBox);  
	m_Button.signal_font_set().connect( sigc::mem_fun(*this, &ExampleWindow::on_button_font_set) );

	m_Button.set_show_size();
	m_Button.set_show_style();
	m_Button.set_use_font();
	m_Button.set_use_size();

	show_all_children();
}

ExampleWindow::~ExampleWindow()
{
}

void ExampleWindow::on_button_font_set()
{
	Glib::ustring font_name = m_Button.get_font_name();
	std::cout << "Font chosen: " << font_name << std::endl;

	Gtk::Label* lbl = Gtk::manage( new Gtk::Label("Example") );
	Pango::FontDescription fdsc(font_name);
	//fdsc.set_size(15000);

	lbl->modify_font(fdsc);

	m_vBox.pack_end( *lbl );

	m_fixedLabel.modify_font(fdsc);

	modify_font( fdsc );
	show_all_children();
}
