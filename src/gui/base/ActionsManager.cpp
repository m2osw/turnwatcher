//===============================================================================
// Copyright (c) 2005-2013 by Made to Order Software Corporation
// 
// All Rights Reserved.
// 
// The source code in this file ("Source Code") is provided by Made to Order Software Corporation
// to you under the terms of the GNU General Public License, version 2.0
// ("GPL").  Terms of the GPL can be found in doc/GPL-license.txt in this distribution.
// 
// By copying, modifying or distributing this software, you acknowledge
// that you have read and understood your obligations described above,
// and agree to abide by those obligations.
// 
// ALL SOURCE CODE IN THIS DISTRIBUTION IS PROVIDED "AS IS." THE AUTHOR MAKES NO
// WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
// COMPLETENESS OR PERFORMANCE.
//===============================================================================

#include "ActionsManager.h"
#include "StatManager.h"
#include "InitiativeManager.h"

#include "pixmaps/d20_init.xpm"
#include "pixmaps/d20_spot.xpm"
#include "pixmaps/d20_listen.xpm"
#include "pixmaps/d20_will.xpm"
#include "pixmaps/add.xpm"
#include "pixmaps/edit.xpm"
#include "pixmaps/delete.xpm"
#include "pixmaps/damage.xpm"
#include "pixmaps/delay.xpm"
#include "pixmaps/end.xpm"
#include "pixmaps/jump_in.xpm"
#include "pixmaps/next.xpm"
#include "pixmaps/ready.xpm"
#include "pixmaps/start.xpm"
#include "pixmaps/website.xpm"

using namespace molib;
using namespace Attribute;
using namespace Initiative;

namespace Actions
{


Manager::private_pointer_t	Manager::f_instance;


/** \class Desensitizer
 *
 ** \brief Disables all actions during a modal operation
 *
 * You sometimes do not want the menus accessed during a modal operation (for example,
 * on Mac OS/X (DARWIN), the menus stay live even while a modal dialog is showing.
 */
 
/** \brief Default constructor
 *
 * Typically used on the stack, this object will keep the UI actions (menus and toolbars) locked
 * while in scope.
 */
Desensitizer::Desensitizer()
{
#ifdef DEBUG
	std::cerr << "Desensitizer::Desensitizer()" << std::endl;
#endif
	PushAll();
}


/** \brief Destructor
 *
 * Restores senisitivity to all actions.
 */
Desensitizer::~Desensitizer()
{
#ifdef DEBUG
	std::cerr << "Desensitizer::~Desensitizer()" << std::endl;
#endif

	PopAll();

#ifdef DEBUG
	std::cerr << "*** Done resetting sensitizers" << std::endl;
#endif
}


void Desensitizer::PushAll()
{
	std::vector<motk::ActionPtr> actions
		= Manager::Instance().lock()->GetActions();
	std::for_each( actions.begin(), actions.end(),
		sigc::mem_fun( *this, &Desensitizer::DesensitizeAction ) );
}


void Desensitizer::PopAll()
{
	std::for_each( m_actionStates.begin(), m_actionStates.end(),
				   sigc::mem_fun( *this, &Desensitizer::SensitizeAction ) );
}


/** \brief Sets an action to a sensitivity state
 *
 * Desensitizes the passed in action to the passed in sensitivity state.
 *
 ** \sa PushAll
 */
void Desensitizer::DesensitizeAction( motk::ActionPtr action )
{
	if( action )
	{
		// Remember the previous state
		//
		m_actionStates[action->get_name()] = action->get_sensitive();
		//
		action->set_sensitive( false );
	}
}


/** \brief Sets an action to a sensitivity state
 *
 * Restores the passed in action to the passed in sensitivity state.
 *
 ** \sa PopAll
 */
void Desensitizer::SensitizeAction( ActionStates::value_type pair )
{
     motk::ActionPtr	action ( Manager::Instance().lock()->GetAction( pair.first ) );
	//
	if( action )
	{
		const bool sensitive = pair.second;
		action->set_sensitive( sensitive );
	}
}


Manager::Manager()
	: motk::ActionManager(Gtk::ICON_SIZE_LARGE_TOOLBAR)
	, f_startImage           (moName("START"   ))
	, f_endImage             (moName("END"     ))
	, f_nextImage            (moName("NEXT"    ))
	, f_jumpInImage          (moName("JUMPIN"  ))
	, f_delayImage           (moName("DELAY"   ))
	, f_readyImage           (moName("READY"   ))
	, f_damageImage          (moName("DAMAGE"  ))
	, f_addImage             (moName("ADD"     ))
	, f_editImage            (moName("EDIT"    ))
	, f_deleteImage          (moName("DELETE"  ))
	, f_d20InitImage         (moName("INIT"    ))
	, f_d20SpotImage         (moName("SPOT"    ))
	, f_d20ListenImage       (moName("LISTEN"  ))
	, f_d20WillImage         (moName("WILL"    ))
	, f_websiteImage         (moName("WEBSITE" ))
	, f_menubarId            (0)
	, f_mainToolbarId        (0)
	, f_startEndButtonId     (0)
	, f_softColumnsMenuId    (0)
	, f_softColumnsToolbarId (0)
	, f_effectsToolbarId     (0)
	, f_hudMenubarId         (0)
{
	motk::ActionManager::ButtonImageSPtr d20InitImage  ( new motk::ActionManager::ButtonImage(d20_init_xpm  ) );
	motk::ActionManager::ButtonImageSPtr d20SpotImage  ( new motk::ActionManager::ButtonImage(d20_spot_xpm  ) );
	motk::ActionManager::ButtonImageSPtr d20ListenImage( new motk::ActionManager::ButtonImage(d20_listen_xpm) );
	motk::ActionManager::ButtonImageSPtr d20WillImage  ( new motk::ActionManager::ButtonImage(d20_will_xpm  ) );
	//
	f_images[f_d20InitImage  ] = d20InitImage  ;
	f_images[f_d20SpotImage  ] = d20SpotImage  ;
	f_images[f_d20ListenImage] = d20ListenImage;
	f_images[f_d20WillImage  ] = d20WillImage  ;
	//
	// Bad idea to call this here since it needs the stat mgr, which sets up an infinite loop
	//
	//CreateActions();
}


Manager::~Manager()
{
	f_images.clear();
	motk::ActionManager::RemoveActions();
	motk::UIBase::Clear();
}


Manager::pointer_t Manager::Instance()
{
	if( !f_instance )
	{
		f_instance.reset( new Manager );
	}
	//
	return f_instance;
}


void Manager::Release()
{
	f_instance.reset();
}


void Manager::ActivateAction( const Glib::ustring& name )
{
	motk::ActionPtr action = f_refActionGroup->get_action( name );
	action->activate();
}


bool Manager::ToggleAction( const Glib::ustring& name )
{
	motk::ToggleActionPtr action = motk::ToggleActionPtr::cast_dynamic( f_refActionGroup->get_action( name ) );
	if( action )
	{
		action->toggled();
	}
	return action;
}

motk::ActionPtr Manager::GetAction( const Glib::ustring& name )
{
	return f_refActionGroup->get_action( name );
}

std::vector<motk::ActionPtr> Manager::GetActions()
{
	return f_refActionGroup->get_actions();
}
	
	
void Manager::SetSensitive( const bool sensitive )
{
	f_refActionGroup->set_sensitive( sensitive );
}


void Manager::AddSoftActions()
{
	typedef std::vector<Manager::ButtonImageSPtr>	ButtonVector;
	ButtonVector	softColImages;
	softColImages.push_back( f_images[f_d20SpotImage  ] );
	softColImages.push_back( f_images[f_d20ListenImage] );
	softColImages.push_back( f_images[f_d20WillImage  ] );
	softColImages.push_back( f_images[f_d20InitImage  ] );
	//
	auto map = StatManager::Instance().lock()->GetStats();

	ButtonVector::iterator			button_iter = softColImages.begin();
	const ButtonVector::iterator	button_end  = softColImages.end();
	//
	for( auto statPair : map )
	{
		auto stat = statPair.second;
		const bool addAction	=  !stat->internal()
								&& !stat->deleted() 
								&& stat->showOnToolbar();
		if( addAction )
		{
			const Glib::ustring	name	( stat->name()    );
			const Glib::ustring	event	( "Roll::" + name );
			const Glib::ustring	accel	( stat->accel()   );
#ifdef DEBUG
			std::cerr << "Adding custom stat: " << name.c_str()
					<< ", event: " << event.c_str() 
					<< ", accel: " << accel.c_str()
					<< std::endl;
#endif
			//
			if( !GetAction( name ) )
			{
				AddAction( name, event, *button_iter, accel );
			}
			//
			if( ++button_iter == button_end )
			{
				// Reset to beginning
				//
				button_iter = softColImages.begin();
			}
		}
	};
}


void Manager::PurgeDeletedActions()
{
	auto map = StatManager::Instance().lock()->GetStats();
	//
	for( auto statPair : map )
	{
		auto stat( statPair.second );
		if( stat->deleted() )
		{
			const Glib::ustring name	( stat->name()    );
			const Glib::ustring event	( "Roll::" + name );
			//
			RemoveAction( event );
		}
	};
}


void Manager::HandleSoftColumnChange()
{
	PurgeDeletedActions();
	AddSoftActions();
}


void Manager::CreateActions()
{
	motk::UIBase::SetActionGroup( f_refActionGroup );

	//===============================================================================
	// FILE MENU
	//===============================================================================
	AddMenu  ( gettext("_File"), "File" );
	AddAction( gettext("_Import"), 		"File::Import", 	 Gtk::Stock::OPEN,			"<control>i" );
	AddAction( gettext("_Export"), 		"File::Export", 	 Gtk::Stock::SAVE,			"<control>e" );
	AddAction( gettext("_Clear"),  		"File::Clear",  	 Gtk::Stock::CLEAR );
	AddAction( gettext("_Quit"),   		"File::Quit",   	 Gtk::Stock::QUIT,			"<control>q" );
	AddAction( gettext("_Preferences"),	"File::Preferences", Gtk::Stock::PREFERENCES,	"<control>p" );
	AddAction( gettext("_Stat Editor"), "File::StatManager", Gtk::Stock::PREFERENCES,	"<control><shift>c" );

	//===============================================================================
	// EDIT MENU
	//===============================================================================
	motk::ActionManager::ButtonImageSPtr addImage   ( new motk::ActionManager::ButtonImage( add_xpm    ) );
	motk::ActionManager::ButtonImageSPtr editImage  ( new motk::ActionManager::ButtonImage( edit_xpm   ) );
	motk::ActionManager::ButtonImageSPtr deleteImage( new motk::ActionManager::ButtonImage( delete_xpm ) );
	f_images[f_addImage 	] = addImage 	;
	f_images[f_editImage	] = editImage	;
	f_images[f_deleteImage	] = deleteImage;
	//
	AddMenu  ( gettext("_Edit")	, "Edit" );
	AddAction( gettext("_Undo")	, "Edit::Undo"		, Gtk::Stock::UNDO,	"<control>z" );
	AddAction( gettext("_Redo")	, "Edit::Redo"		, Gtk::Stock::REDO,	"<control>y" );
	AddAction( gettext("_Add")	, "Edit::Add"		, addImage,		"<control>a" );
	AddAction( gettext("Ed_it")	, "Edit::Edit"		, editImage,		"<control>j" );
	AddAction( gettext("_Delete")	, "Edit::Delete"	, deleteImage,		"<control>x" );
	AddAction( gettext("Duplica_te"), "Edit::Duplicate"	, 0,			"<control>l" );
	AddAction( gettext("_Purge Dead Characters"), "Edit::PurgeDead", 0,	"<control><shift>p" );
	//
	AddAction( gettext("Add E_ffect")	, "Edit::AddEffect"	, Gtk::Stock::ADD	, "<control><shift>a" );
	AddAction( gettext("Edit Effe_ct")	, "Edit::EditEffect"	, Gtk::Stock::EDIT	, "<control><shift>e" );
	AddAction( gettext("De_lete Effect")	, "Edit::DeleteEffect"	, Gtk::Stock::DELETE	, "<control><shift>x" );

	//===============================================================================
	// VIEW MENU
	//===============================================================================
	AddMenu  ( gettext("_View"), "View" );
	AddToggleAction( gettext("Show _Toolbar"),      "View::ShowToolbar",     0, "<control>t" );
	AddToggleAction( gettext("Toolbar on _Bottom"), "View::ToolbarOnBottom", 0, "<control>b" );
	AddToggleAction( gettext("Show _Effects Pane"), "View::ShowEffects",     0, "<control>f" );
#ifdef WANT_NOTES
	AddToggleAction( gettext("Show _Info Window"), "View::ShowInfo"    ,     0, "<control>n" );
#endif
	AddToggleAction( gettext("Show _HUD Window"), "View::ShowHUD"	   ,	 0, "<control>h" );

	//===============================================================================
	// ROLL MENU
	//===============================================================================

	AddMenu  ( gettext("_Roll"), "Roll" );

#ifdef DARWIN
	AddAction( gettext("_Initiative"), "Roll::Initiative", f_images[f_d20InitImage]  , "<control>i" );
#else
	AddAction( gettext("_Initiative"), "Roll::Initiative", f_images[f_d20InitImage]  , "i" );
#endif

	AddSoftActions();

	//===============================================================================
	// ROUNDS MENU
	//===============================================================================
	motk::ActionManager::ButtonImageSPtr startImage  = new motk::ActionManager::ButtonImage( start_xpm	);
	motk::ActionManager::ButtonImageSPtr endImage    = new motk::ActionManager::ButtonImage( end_xpm 	);
	motk::ActionManager::ButtonImageSPtr nextImage   = new motk::ActionManager::ButtonImage( next_xpm	);
	motk::ActionManager::ButtonImageSPtr jumpInImage = new motk::ActionManager::ButtonImage( jump_in_xpm	);
	motk::ActionManager::ButtonImageSPtr delayImage  = new motk::ActionManager::ButtonImage( delay_xpm	);
	motk::ActionManager::ButtonImageSPtr readyImage  = new motk::ActionManager::ButtonImage( ready_xpm	);
	motk::ActionManager::ButtonImageSPtr damageImage = new motk::ActionManager::ButtonImage( damage_xpm	);
	//
	f_images[f_startImage	] = startImage ;
	f_images[f_endImage	] = endImage   ;
	f_images[f_nextImage	] = nextImage  ;
	f_images[f_jumpInImage	] = jumpInImage;
	f_images[f_delayImage	] = delayImage ;
	f_images[f_readyImage	] = readyImage ;
	f_images[f_damageImage	] = damageImage;

	AddMenu  ( gettext("R_ounds"), "Rounds" );
#ifdef DARWIN
	AddAction( gettext("_Start"),   "Rounds::Start",  startImage,	"<control>g"	);
	AddAction( gettext("_End"),     "Rounds::End",    endImage,		"<control>e"	);
	AddAction( gettext("_Next"),    "Rounds::Next",   nextImage,	"space"			);
	AddAction( gettext("_Delay"),   "Rounds::Delay",  delayImage,	"<control>d"	);
	AddAction( gettext("_Ready"),   "Rounds::Ready",  readyImage,  	"<control>r"	);
	AddAction( gettext("_Jump In"), "Rounds::JumpIn", jumpInImage,	"<control>j"	);
	AddAction( gettext("Da_mage"),  "Rounds::Damage", damageImage, 	"<control>m"	);
	//
	AddToggleAction( gettext("Stabili_ze")   , "Rounds::Stabilize", 0, "<control>b" );
#else
	AddAction( gettext("_Start"),   "Rounds::Start",  startImage,	"g"	);
	AddAction( gettext("_End"),     "Rounds::End",    endImage,		"e"	);
	AddAction( gettext("_Next"),    "Rounds::Next",   nextImage,	"space" );
	AddAction( gettext("_Delay"),   "Rounds::Delay",  delayImage,	"d"	);
	AddAction( gettext("_Ready"),   "Rounds::Ready",  readyImage,  	"r"	);
	AddAction( gettext("_Jump In"), "Rounds::JumpIn", jumpInImage,	"j"	);
	AddAction( gettext("Da_mage"),  "Rounds::Damage", damageImage, 	"m"	);
	//
	AddToggleAction( gettext("Stabili_ze")   , "Rounds::Stabilize", 0, "b" );
#endif
	//
	AddAction( gettext("Move _Up")  ,	"Rounds::MoveUp",	Gtk::Stock::GO_UP );
	AddAction( gettext("Move _Down"),	"Rounds::MoveDown",	Gtk::Stock::GO_DOWN );

	//===============================================================================
	// HELP MENU
	//===============================================================================
	motk::ActionManager::ButtonImageSPtr websiteImage = new motk::ActionManager::ButtonImage( website_xpm );
	f_images[f_websiteImage] = websiteImage;
	//
	AddMenu  ( gettext("_Help"), "Help" );
	AddAction( gettext("_About"),         "Help::About",         Gtk::Stock::ABOUT,   "<control><shift>h" );
	AddAction( gettext("_Documentation"), "Help::Documentation", Gtk::Stock::INFO,    "F1"                );
	AddAction( gettext("_Tutorial"),      "Help::Tutorial",      Gtk::Stock::HELP,    "F2"                );
	AddAction( gettext("_Contact"),       "Help::Contact",       Gtk::Stock::CONNECT, "F3"                );
	AddAction( gettext("_License"),       "Help::License",       Gtk::Stock::DND,     "F4"                );
	AddAction( gettext("Turn Watcher _Home Page"), "Help::HomePage",      websiteImage,       "<control><shift>t" );

	//===============================================================================
	// HUD POPUP
	//===============================================================================
	AddMenu  ( gettext("Heads _Up Display"), "HUD" );
	AddAction( gettext("Change _Font"),	"HUD::ChangeFont",	Gtk::Stock::SELECT_FONT,	"<control><shift>f" );
}


Glib::ustring Manager::GetSoftMenus( const bool toolitem )
{
	typedef std::map<int,Attribute::Stat::pointer_t> StatMap;
	StatMap statMap;
	//
	auto map = StatManager::Instance().lock()->GetStats();
	for( auto statPair : map )
	{
		auto stat( statPair.second );
		if( !stat->deleted() )
		{
			statMap[stat->order()] = stat;
		}
	};

	f_softColumnsOrder.clear();
	//
	Glib::ustring ui;
	//
	for( auto statPair : statMap )
	{
		auto stat( statPair.second );
		if( stat->showOnToolbar() )
		{
			Glib::ustring action = Glib::ustring("Roll::") + stat->name();
			//
			if( toolitem )
			{
				ui += "<toolitem action='" + action + "'/>\n";
			}
			else
			{
				ui += "<menuitem action='" + action + "'/>\n";
			}
			//
			f_softColumnsOrder.push_back( action );
		}
	};
	return ui;
}


Gtk::Widget* Manager::GetMenubar()
{
	if( f_menubarId )
	{
		UIBase::RemoveUI( f_menubarId );
	}

	Glib::ustring	ui;
	ui  = "<ui>\n";
	ui += "  <menubar action='MenuBar'>\n";
	ui += "    <menu action='File'>\n";
	ui += "      <menuitem action='File::Import'/>\n";
	ui += "      <menuitem action='File::Export'/>\n";
	ui += "      <separator/>\n";
	ui += "      <menuitem action='File::Clear'/>\n";
	ui += "      <separator/>\n";
	ui += "      <menuitem action='File::Preferences'/>\n";
	ui += "      <menuitem action='File::StatManager'/>\n";
	ui += "      <separator/>\n";
#ifndef DARWIN
	ui += "      <menuitem action='File::Quit'/>\n";
#endif
	ui += "    </menu>\n";
	ui += "    <menu action='Edit'>\n";
	ui += "      <menuitem action='Edit::Undo'/>\n";
	ui += "      <menuitem action='Edit::Redo'/>\n";
	ui += "      <separator/>\n";
	ui += "      <menuitem action='Edit::Add'/>\n";
	ui += "      <menuitem action='Edit::Edit'/>\n";
	ui += "      <menuitem action='Edit::Delete'/>\n";
	ui += "      <separator/>\n";
	ui += "      <menuitem action='Edit::Duplicate'/>\n";
	ui += "      <menuitem action='Edit::PurgeDead'/>\n";
	ui += "      <separator/>\n";
	ui += "      <menuitem action='Edit::AddEffect'/>\n";
	ui += "      <menuitem action='Edit::EditEffect'/>\n";
	ui += "      <menuitem action='Edit::DeleteEffect'/>\n";
	ui += "      <separator/>\n";
	ui += "    </menu>\n";
	ui += "    <menu action='View'>\n";
	ui += "      <menuitem action='View::ShowToolbar'/>\n";
	ui += "      <menuitem action='View::ToolbarOnBottom'/>\n";
	ui += "      <menuitem action='View::ShowEffects'/>\n";
#ifdef WANT_NOTES
	ui += "      <menuitem action='View::ShowInfo'/>\n";
#endif
	ui += "      <menuitem action='View::ShowHUD'/>\n";
	ui += "    </menu>\n";
	ui += "    <menu action='Rounds'>\n";
	ui += "      <menuitem action='Rounds::Start'/>\n";
	ui += "      <menuitem action='Rounds::End'/>\n";
	ui += "      <menuitem action='Rounds::Next'/>\n";
	ui += "      <menuitem action='Rounds::Delay'/>\n";
	ui += "      <menuitem action='Rounds::Ready'/>\n";
	ui += "      <menuitem action='Rounds::JumpIn'/>\n";
	ui += "      <menuitem action='Rounds::Damage'/>\n";
	ui += "      <menuitem action='Rounds::Stabilize'/>\n";
	ui += "      <separator/>\n";
	ui += "      <menuitem action='Rounds::MoveUp'/>\n";
	ui += "      <menuitem action='Rounds::MoveDown'/>\n";
	ui += "      <separator/>\n";
	ui += "    </menu>\n";
	ui += "    <menu action='Roll'>\n";
	ui += "      <menuitem action='Roll::Initiative'/>\n";
	ui += "      <separator/>\n";
	ui += "      <placeholder name='SoftColumns'/>\n";
	ui += "    </menu>\n";
	ui += "    <menu action='Help'>\n";
	ui += "      <menuitem action='Help::About'/>\n";
	ui += "      <separator/>\n";
	ui += "      <menuitem action='Help::Documentation'/>\n";
	ui += "      <menuitem action='Help::Tutorial'/>\n";
	ui += "      <menuitem action='Help::Contact'/>\n";
	ui += "      <menuitem action='Help::License'/>\n";
	ui += "      <menuitem action='Help::HomePage'/>\n";
	ui += "    </menu>\n";
	ui += "  </menubar>\n";
	ui += "</ui>\n";
	//
#ifdef DEBUG
	std::cerr << "menu: " << ui.c_str() << std::endl;
#endif
	//
	f_menubarId = UIBase::AddUI( ui );
	
	AlterSoftColumnsMenu();
	
	return UIBase::GetWidget( "/MenuBar" );
}


void Manager::AlterSoftColumnsMenu()
{
	if( f_softColumnsMenuId )
	{
		UIBase::RemoveUI( f_softColumnsMenuId );
	}
	//
	Glib::ustring ui  = "<ui>\n";
	ui += "  <menubar action='MenuBar'>\n";
	ui += "    <menu action='Roll'>\n";
	ui += "      <placeholder name='SoftColumns'>\n";
	ui += 			GetSoftMenus( false /*toolbar*/ );
	ui += "      </placeholder>\n";
	ui += "    </menu>\n";
	ui += "  </menubar>\n";
	ui += "</ui>\n";
	//
	f_softColumnsMenuId = UIBase::AddUI( ui );
}


Gtk::Widget* Manager::GetMainToolbar()
{
	if( f_mainToolbarId )
	{
		UIBase::RemoveUI( f_mainToolbarId );
	}
	
	Glib::ustring ui;
	ui  = "<ui>\n";
	ui += "  <toolbar action='MainToolbar'>\n";
	ui += "    <toolitem action='Edit::Add'/>\n";
	ui += "    <toolitem action='Edit::Edit'/>\n";
	ui += "    <toolitem action='Edit::Delete'/>\n";
	ui += "    <separator/>\n";
	ui += "    <placeholder name='StartEndButtons'>\n";
	ui += "    </placeholder>\n";
	ui += "    <toolitem action='Rounds::Next'/>\n";
	ui += "    <toolitem action='Rounds::Delay'/>\n";
	ui += "    <toolitem action='Rounds::Ready'/>\n";
	ui += "    <toolitem action='Rounds::JumpIn'/>\n";
	ui += "    <toolitem action='Rounds::Damage'/>\n";
	ui += "    <separator/>\n";
	ui += "    <toolitem action='Roll::Initiative'/>\n";
	ui += "    <separator/>\n";
	ui += "    <placeholder name='SoftColumns'/>\n";
	ui += "  </toolbar>\n";
	ui += "</ui>\n";

#ifdef DEBUG
	std::cerr << "toolbar: " << ui.c_str() << std::endl;
#endif
	//
	f_mainToolbarId = UIBase::AddUI( ui );
	
	AlterStopStartButtonToolbar();
	AlterSoftColumnsToolbar();

	Gtk::Toolbar* toolbar = dynamic_cast<Gtk::Toolbar*>( UIBase::GetWidget( "/MainToolbar" ) );
	toolbar->set_icon_size( Gtk::ICON_SIZE_LARGE_TOOLBAR );
	toolbar->set_toolbar_style( Gtk::TOOLBAR_BOTH );

	return toolbar;
}


void Manager::AlterStopStartButtonToolbar()
{
	if( f_startEndButtonId )
	{
		UIBase::RemoveUI( f_startEndButtonId );
	}
	//
	auto initMgr = InitiativeManager::Instance().lock();
	Glib::ustring ui  = "<ui>\n";
	ui += "  <toolbar action='MainToolbar'>\n";
	ui += "    <placeholder name='StartEndButtons'>\n";
	if( initMgr->InRounds() )
	{
		ui += "    <toolitem action='Rounds::End'/>\n";
	}
	else
	{
		ui += "    <toolitem action='Rounds::Start'/>\n";
	}
	//
	ui += "    </placeholder>\n";
	ui += "  </toolbar>\n";
	ui += "</ui>\n";
	//
	f_startEndButtonId = UIBase::AddUI( ui );
}


void Manager::AlterSoftColumnsToolbar()
{
	if( f_softColumnsToolbarId )
	{
		UIBase::RemoveUI( f_softColumnsToolbarId );
	}
	//
	Glib::ustring ui  = "<ui>\n";
	ui += "  <toolbar action='MainToolbar'>\n";
	ui += "    <placeholder name='SoftColumns'>\n";
	ui += 			GetSoftMenus( true /*toolbar*/ );
	ui += "    </placeholder>\n";
	ui += "  </toolbar>\n";
	ui += "</ui>\n";
	//
	f_softColumnsToolbarId = UIBase::AddUI( ui );
	
	// UGLY HACK!
	// This is done because of a bug in the Gtk UIManager code. Even though
	// we've reordered the menus in the XML above, they remain fixed until
	// the application is restarted.
	//
	// I need to report this at some point to the Gtk+/mm team.
	//
	int soft_idx = 1;
	int init_idx = 0;
	for( auto order : f_softColumnsOrder )
	{	
		Gtk::Toolbar* toolbar = dynamic_cast<Gtk::Toolbar*>( UIBase::GetWidget( "/MainToolbar" ) );
		const int toolcount = toolbar->get_n_items();
		for( int idx = 0; idx < toolcount; ++idx )
		{
			Gtk::ToolButton* item = dynamic_cast<Gtk::ToolButton*>(toolbar->get_nth_item( idx ));
			if( item )
			{
				Glib::ustring action_id = item->get_stock_id();
				//
				if( action_id == "Roll::Initiative" )
				{
					init_idx = idx + 1; // Take into account the separator after the initiative entry
					continue;
				}
				//
				if( action_id == order )
				{
					toolbar->remove( *item );
					toolbar->insert( *item, init_idx + soft_idx );
				}
			}
		}
		//
		soft_idx++;
	}			
}


Gtk::Widget* Manager::GetEffectsToolbar()
{
	if( f_effectsToolbarId )
	{
		UIBase::RemoveUI( f_effectsToolbarId );
	}
	
	Glib::ustring ui;
	ui  = "<ui>\n";
	ui += "  <toolbar action='EffectsToolbar'>\n";
	ui += "    <toolitem action='Edit::AddEffect'/>\n";
	ui += "    <toolitem action='Edit::EditEffect'/>\n";
	ui += "    <toolitem action='Edit::DeleteEffect'/>\n";
	ui += "  </toolbar>\n";
	ui += "</ui>\n";

	f_effectsToolbarId = UIBase::AddUI( ui );
	
	Gtk::Toolbar* toolbar = dynamic_cast<Gtk::Toolbar*>( UIBase::GetWidget( "/EffectsToolbar" ) );
	toolbar->set_icon_size( Gtk::ICON_SIZE_SMALL_TOOLBAR );
	toolbar->set_toolbar_style( Gtk::TOOLBAR_ICONS );
	toolbar->set_show_arrow( false );

	return toolbar;
}


void Manager::AlterUI()
{
	AlterSoftColumnsMenu();
	AlterSoftColumnsToolbar();
	AlterStopStartButtonToolbar();
}


Gtk::Widget* Manager::GetHUDMenu()
{
	if( f_hudMenubarId )
	{
		UIBase::RemoveUI( f_hudMenubarId );
	}

	Glib::ustring	ui;
	ui  = "<ui>\n";
	ui += "  <popup action='PopupMenu'>\n";
	ui += "      <menuitem action='View::ShowHUD'/>\n";
	ui += "		 <separator/>\n";
	ui += "      <menuitem action='HUD::ChangeFont'/>\n";
	ui += "  </popup>\n";
	ui += "</ui>\n";

	f_hudMenubarId = UIBase::AddUI( ui );

	return UIBase::GetWidget( "/PopupMenu" );
}


void Manager::OnButtonClicked( Glib::RefPtr<Gtk::Action>& action, const Glib::ustring& event )
{
#ifdef DEBUG
	std::cerr << "Actions::Manager::OnButtonClicked(): " << event << std::endl;
#endif
}


}

// vim: ts=4 sw=4

