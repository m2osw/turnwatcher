//===============================================================================
// Copyright (c) 2005-2017 by Made to Order Software Corporation
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



#ifdef MO_PRAGMA_INTERFACE
#pragma implementation "mo/mo_application.h"
#endif

#include	"mo/mo_application.h"

#ifndef MO_PROPS_XML_H
#include	"mo/mo_props_xml.h"
#endif
#ifndef MO_FILE_H
#include	"mo/mo_file.h"
#endif

#if defined(MO_LINUX) || defined(LINUX)
#include	<libintl.h>
#endif

#if defined(MO_WIN32) || defined(WIN32)
#include	<shlobj.h>
#include	<libintl.h>
#include	<memory>
#endif

#if defined(MO_DARWIN) || defined(MAC_I386)
#include	<libintl.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif


namespace molib
{


// We can't use a smart pointer for a static variable because we
// can't garantee that it will be initialized before it is first
// used (and thus you could end-up with two applications!)
//
moApplication *		moApplication::g_instance;
zbool_t			moApplication::g_done;


/** \brief  Return the singleton application instance
 *
 * This function returns the application instance. It is
 * a static function which creates a singleton.
 * 
 * An application instance is unique.
 * 
 * Once you are done with this singleton, call the Done() function. It
 * will release the application instance and mark the function as done
 * (i.e. the Instance() function will fail).
 *
 * \bug
 * The Instance() function is NOT multi-thread safe when first
 * called. We assume that the function will be called at least
 * once before threads which has a chance to also call this
 * function are created.
 *
 * \return Returns a smart pointer to an application instance
 *
 * \sa Done, SetArgs, Run, Terminate
 */
moApplicationSPtr moApplication::Instance(void)
{
	if(!g_instance) {
		// if we're done, we don't want to create a new instance
		if(g_done) {
			return 0;
		}
		g_instance = new moApplication;
		g_instance->AddRef();
	}

	return g_instance;
}

 
/** \brief Release the application pointer to signal application end
 *
 * Release the application object for the last time.
 *
 * \bug
 * The Done() function is NOT multi-thread safe. It should be
 * called after all the child threads joined the parent thread.
 *
 * \sa Instance
 */
void moApplication::Done(void)
{
	if(!g_done) {
		g_done = true;
		g_instance->Release();
		g_instance = 0;
	}
}


/** \brief Initializes an moApplication object.
 *
 * This function is private. You can at most have one
 * application instance. This one is retrieved with
 * the Instance() function call. That function will
 * have the right to create an moApplication object.
 *
 * \sa Instance
 */
moApplication::moApplication(void)
	: f_run(true),
	  f_configuration("ApplicationConfiguration"),
	  f_application_version("1.0"),
	  f_manufacturer("Made to Order Software Corporation")
{
	SetRootPath(getenv("MO_ROOTPATH"));	/* Flawfinder: ignore */
}


/** \brief Clean up the Application object.
 *
 * This function ensures that the resources used by the
 * application object are released.
 */
moApplication::~moApplication()
{
}

// documented in moBase
const char *moApplication::moGetClassName() const
{
	return "moBase::moEventPipe::moEventPipeBroadcast::moApplication";
}


/** \brief Set the name of the application.
 *
 * The name and version will be used as the default for
 * different directories and filenames such as the
 * ~/.\<name>-\<version> user directory.
 *
 * \bug Changing the application name after initial set could have side
 * effects.  It is strongly suggested that you don't.
 *
 * \param[in] name   The name of the application
 *
 * \sa SetVersion, GetVersion, GetName, GetFilename
 */
void moApplication::SetName(const moWCString& name)
{
	f_application_name = name;
	DefineApplicationFilename();

#ifdef WIN32
	// under Win32, gettext() doesn't get any magically proper initialization
	// so we do it by hand here.
	//
	// WARNING: we assume that the input files are UTF-8 (and they should be)
	//
	// The path goes inside share/turnwatcher
	// We remove /turnwatcher
	// We add /locale
	// Then we revert all the / into \ because gettext() doesn't understand / under Win32 (!)
	moWCString path = GetApplicationPath();
	path = path.FilenameDirname().FilenameChild("locale").Replace("/=\\\\");
//printf("Binding with [%s]\n", path.SavedMBData());
	bindtextdomain(f_application_name.SavedMBData(), path.SavedMBData());
#endif

	bind_textdomain_codeset(f_application_name.SavedMBData(), "utf-8");
	textdomain(f_application_name.SavedMBData());
}


/** \brief Get the name of the application.
 *
 * This function returns a constant reference to the application
 * name. If you need to change the name, use the SetName() function.
 *
 * \return A reference to the name string.
 *
 * \sa SetName
 */
const moWCString& moApplication::GetName() const
{
	return f_application_name;
}


/** \brief Set the application version.
 *
 * This defines the version of the user application.
 *
 * Note that even if the version is a number, this function expects
 * a string.
 *
 * \param[in] version   The version in the form of a string.
 *
 * \sa GetVersion, SetName, SetCopyright
 */
void moApplication::SetVersion(const moWCString& version)
{
	f_application_version = version;
}


/** \brief Retrieve the application version.
 *
 * This function returns the current application version
 * in a constant reference string.
 *
 * To change the version, use the SetVersion() function.
 *
 * \return A reference to the version string
 *
 * \sa SetVersion, SetName, SetCopyright
 */
const moWCString& moApplication::GetVersion() const
{
	return f_application_version;
}


/** \brief Sets the copyright string for the application.
 *
 * The copyright line is expected to be one line.
 *
 * \param[in] copyright   The copyright of the application.
 *
 * \sa GetCopyright, SetName, SetVersion
 */
void moApplication::SetCopyright(const moWCString& copyright)
{
	f_application_copyright = copyright;
}


/** \brief Retrieve the copyright of the application.
 *
 * This function retrieves a reference to the copyright string.
 *
 * To change the copyright, call the SetCopyright() function.
 *
 * \return The copyright string.
 *
 * \sa SetCopyright
 */
const moWCString& moApplication::GetCopyright() const
{
	return f_application_copyright;
}


/** \brief Gets a filename based on the application name.
 *
 * This function returns the name of the application fixed
 * to work as a filename (i.e. the spaces are replaced with
 * underscores, letters are changed to lowercase under
 * Unices, etc.) This will always be one name (no slash
 * or backslash).
 *
 * This filename is used for configuration files and temporary
 * files.
 *
 * \return The application filename.
 *
 * \bug
 * The GetFilename() does not yet handle all the possible
 * invalid directory or file names.
 *
 * \sa GetConfiguration, SetName
 */
const moWCString& moApplication::GetFilename() const
{
	return f_application_filename;
}


/** \brief Defines the application filename once.
 *
 * This function defines the application filename each time the
 * application name is changed.
 */
void moApplication::DefineApplicationFilename()
{
	// TODO: we need to replace all the filename invalid characters,
	//	 not just the space.
	f_application_filename = f_application_name.Replace(" =_");
#ifndef WIN32
	// Under Unices, also force all the letters to lowercase
	f_application_filename = f_application_filename.Lowercase();
#endif
}


/** \brief Set the application root path.
 *
 * The root path is used to install the application in a
 * sub-directory which isn't the root (/). For instance
 * you may compile your application in:
 * 
 * \code
 *	/home/alexis/my_app
 * \endcode
 * 
 * And you install it in a sub-directory named BUILD.
 * In this case, the root path should be set to:
 * 
 * \code
 *	/home/alexis/my_app/BUILD
 * \endcode
 * 
 * Note that though the application root path will be set
 * to that path, there is nothing which forces any function
 * other than the ones available in the moApplication
 * to actually make use of that root path. It is up to the
 * application developers to make sure they make use of this
 * path.
 * 
 * You may set the root path three different ways:
 *
 * \li by calling the SetRootPath() function,
 * \li by setting the MO_ROOTPATH environment variable before
 * application start, or
 * \li by using the --root-path command line option.
 *
 * The latter will not be available if you call
 * the SetOptions() function with the use_default_options
 * parameter set to false.
 * 
 * \note
 * SetRootPath() will canonilize the path so it
 * is a valid path for the system you are running on. For
 * instance, under MS-Windows, a drive letter will be prepended
 * if none are specified in the input string. Also, a root path
 * is always forced to start with a slash (/).
 * 
 * \p note2
 * The GetRootPath() should only be used in development while your
 * application doesn't yet reside at the right place on your
 * disk (it will be once you install it). Once your application
 * is installed, the root path automatically becomes a single
 * slash (/).
 *
 * \bug
 * When an MS-DOS root path starts with a drive letter and a
 * colon but no slash (/), then a slash is added right after
 * the colon. This may not be what you expect. It is required
 * that the root path be a full path.
 *
 * \sa GetRootPath, GetConfiguration, GetName, SetName
 */
void moApplication::SetRootPath(const moWCString& root_path)
{
	if(root_path.IsEmpty()) {
#if WIN32
		f_root_path = "C:/";
#else
		f_root_path = "/";
#endif
	}
	else {
		f_root_path = root_path.FilenameClean();

#if WIN32
		// we expect "[A-Z]:/"
		mowc::wc_t c = mowc::toupper(f_root_path[0]);
		// First check whether we already have a drive,
		// but no slash; in this case add the slash only
		if( f_root_path.Length() > 2
		&& (c >= 'A' && c <= 'Z')
		&& f_root_path[1] == mowc::wc_t(':')
		&& f_root_path[2] != mowc::wc_t('/') ) {
			f_root_path.Insert("/", 2);
		}
		else if( f_root_path.Length() < 3
		|| c < 'A' || c > 'Z'
		|| f_root_path[1] != mowc::wc_t(':')
		|| f_root_path[2] != mowc::wc_t('/') ) {
			// TODO: canonilize the path first?
			// We randomly choose the C: drive, should
			// we use the current drive instead?
			f_root_path.Insert("C:/", 0);
		}
#else
		if(f_root_path[0] != '/') {
			f_root_path.Insert("/", 0);
		}
#endif
	}
}


/** \brief Get the application root path.
 *
 * \return A constant string reference to the root path of the application.
 *
 * \sa SetRootPath
 */
const moWCString& moApplication::GetRootPath() const
{
	return f_root_path;
}


/** \brief Set the application path.
 *
 * If you want to use a path to some read-only data specific to
 * your application, you need to use the application path.
 * You can change the application path using the
 * SetApplicationPath() function and retrieve it with the
 * GetApplicationPath() function. This path uses the root path
 * Sub-directories can be defined under the application path
 * using the information in the application configuration
 * file (see GetConfiguration() for more information).
 *
 * \note
 * The directory pointed by the GetApplicationPath()
 * will in most cases be read-only. It should always be
 * considered that way. The sub-directories will be created and
 * initialized and installation time.
 * 
 * By default, the application path is built from a path found
 * in the configuration file and the application name. When
 * the configuration file doesn't define the paths, then the
 * system makes an attempt to find the application directory
 * and uses that instead.
 * 
 * \section Win32
 * Under Microsoft Windows 2K/XP/Vista, the default path is the path to the application
 * less one directory. It is expected that this will be something
 * like this:
 * 
 * \code
 * C:\Program File\<manufacturer>\<application name>\<version>\
 * \endcode
 * 
 * Note that the application binary is expected to be like this:
 * 
 * \code
 * C:\Program File\<manufacturer>\<application name>\<version>\bin\<application name>.exe
 * \endcode
 *
 * \sa GetApplicationPath
 */
void moApplication::SetApplicationPath(const moWCString& application_path)
{
	f_application_path = application_path.FilenameClean();
}


/** \brief The the path of the application data.
 *
 * This function computes a valid path where the application can save
 * data for the currently running instance. This is in general based
 * on the user and some other such parameters.
 *
 * \return Returns a constant string reference to the application path.
 *
 * \sa SetApplicationPath
 */
const moWCString moApplication::GetApplicationPath(bool docs)
{
	// TODO:
	// should we automatically include the root path to the
	// application path?! At this time, it is included only
	// on return, not in the f_application_path variable.
	// This means the root path can be defined after you
	// set the application path and it will still work.

	// If still empty, define a default path. The configuration
	// may also determine a path so we first try to read the
	// configuration file.
	if(f_application_path.IsEmpty()) {
		// we ignore the property bag being returned
		// the function will already define the path
		// if it exists in that file.
		// NOTE: do we really want that under MS-Windows?!
		GetConfiguration();
		// if still empty, try to define a default path
		if(f_application_path.IsEmpty()) {
#ifdef WIN32
			// Under MS-Windows we use the path to the application
			// to dynamically determine where our data can be found.
			std::auto_ptr<wchar_t> module_path;
			DWORD size = MAX_PATH;
			do {
				module_path.reset(new wchar_t[size]);
				module_path.get()[0] = '\0';
				DWORD sz = GetModuleFileNameW(NULL, module_path.get(), size);
				if(sz < size) {
					break;
				}
				if(GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
					// we can't get a property path?!
					wcscpy(module_path.get(), L"C:/");	/* Flawfinder: ignore */
					break;
				}
				size += MAX_PATH;
			} while(size < MAX_PATH * 5);
			wchar_t *s = module_path.get();
			while(*s != '\0') {
				if(*s == '\\') {
					*s = '/';
				}
				s++;
			}
			f_application_path = module_path.get();
			// GetModuleFilename() includes the name of the application
			// we expect the binary to be in a sub-directory named 'bin'
			// and thus the application path is really one up
			f_application_path = f_application_path.FilenameDirname().FilenameDirname();
			f_application_path = f_application_path.FilenameChild("share/" + f_application_filename);
#else
			if(docs) {
				f_application_path = "/usr/share/doc/" + f_application_filename;
			}
			else {
				f_application_path = "/usr/share/" + f_application_filename;
			}
#endif
		}
	}

#ifdef WIN32
	// under MS we really cannot use a root path because of drive specifications
	return f_application_path;
#else
	return f_root_path.FilenameChild(f_application_path);
#endif
}



/** \brief Get the user path.
 *
 * Use this method to obtain the (automatically generated) user
 * path, where access to user-writable folders exist. This
 * path is different depending on the platform.
 *
 * \section Unix-clones
 *
 * Under unix-clone systems (for example, GNU/Linux and FreeBSD),
 * the user path is the environment $HOME.
 *
 * \section Win32
 *
 * Under Microsoft Windows 2K/XP/Vista, we first test for the
 * $HOME environment variable. If you have that variable set, then it will use
 * it exactly like under unix-clone systems (the private directory
 * will be marked hidden at installation time).
 *
 * When the $HOME environment variable doesn't exist, a 
 * Win32 Shell API function is used instead to determine the user path (SHGetFolderPath).
 * That function is called with the option CSIDL_PERSONAL to obtain the private
 * user area.
 * 
 * SHGetFolderPath(CSIDL_PERSONAL) returns the path as follows:
 * 
 * \code
 * C:\My Documents\<application name>
 * \endcode
 * 
 * Note that My Documents is a virtual folder and usually the
 * function will transform that folder name to a physical name.
 * (i.e. C:\\Documents and Settings\\\<username>\\My Documents\\\<appname>)
 *
 * \note
 * It is recommened you call the SetName() function prior to calling
 * this method. This method caches the value the first time it is called.
 * To force re-evaluation of the path, call the ResetUserPath().
 *
 * \sa GetPrivateUserPath, GetVersion, ResetUserPath
 */
const moWCString& moApplication::GetUserPath()
{
	// Note that the user paths are never affected by the root path
	if(f_user_path.IsEmpty()) {
#ifdef WIN32
		// Under MS-Windows you need to call one of the
		// SHGetFolderPath() functions
		//
		// Note that we first test if the HOME environment variable
		// is set and we use that if it does.
		f_user_path = getenv("HOME");	/* Flawfinder: ignore */
		if(f_user_path.IsEmpty()) {
			wchar_t path[MAX_PATH];	/* Flawfinder: ignore */
			HRESULT hr = SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, 0, path);
			if(hr == NOERROR) {
				f_user_path = path;
				f_user_path = f_user_path.FilenameChild(f_application_filename);
			}
			else {
				// it failed?! (should we throw?)
				f_user_path = "C:/";
			}
		}
#else
		// Under Unix, it is really rare that the HOME environment
		// variable is not set; also, I have never seen it been
		// wrongly set on a properly working system
		f_user_path = getenv("HOME");	/* Flawfinder: ignore */
#endif
		moFile::CreateDir( f_user_path, 0700 );
	}

	return f_user_path;
}


/** \brief Get the private user path.
 *
 * Use this method to obtain the (automatically generated) private user
 * path, where access to user-writable folders exist. You should use this
 * path to store application configuration files that the user should not
 * have obvious access to. This path is different depending on the platform.
 *
 * \section Unix-clones
 *
 * Under unix-clone systems (for example, GNU/Linux and FreeBSD),
 * the private user path is built from $HOME,
 * the name of the application and the version:
 * 
 * \code
 *	$HOME/.<application name>-<version>
 * \endcode
 *
 * \section Win32
 *
 * Under Microsoft Windows 2K/XP/Vista, we first test for the
 * $HOME environment variable. If you have that variable set,
 * then it will use it exactly like under unix-clone systems
 * (the private directory will be marked hidden at installation
 * time).
 * 
 * When the $HOME environment variable doesn't exist, a 
 * Win32 Shell API function is used instead to determine the
 * user path (SHGetFolderPath).
 * 
 * In general, the CSIDL_APPDATA will translate to a hidden
 * folder as follows:
 * 
 * \code
 * C:\Documents and Settings\<username>\Application Data\<manufacturer>\<application name>\<version>\
 * \endcode
 * 
 * \note
 * It is recommended that you first set the application name, version and
 * manufacturer prior to calling this method. This method caches the private
 * user name the first time it is called. If you wish to reevaluate the path,
 * (for example to access previous version config files of your application)
 * call the ResetPrivateUserPath below to start again.
 *
 * \param[in] append_version	Setting to true will embed the version number
 *				in the private user path.
 *
 * \sa GetUserPath, GetVersion, ResetPrivateUserPath, SetManufacturer
 */
const moWCString& moApplication::GetPrivateUserPath(bool append_version)
{
	if(f_application_filename.IsEmpty()) {
		throw moError("GetPrivateUserPath() not supported without an application name");
	}

	// Note that the user paths are never affected by the root path
	if(f_private_user_path.IsEmpty()) {
#ifdef WIN32
		// Under MS-Windows you need to call a GetFolder() function
		//
		// Also, the user hidden data can be in a totally different
		// place than the visible one and thus we can't use
		// GetUserPath() to deterimne that place.
		//
		// If the $HOME variable exists, we want to use it just
		// like under Unix.
		moWCString home = getenv("HOME");	/* Flawfinder: ignore */
		if(home.IsEmpty()) {
			wchar_t path[MAX_PATH];	/* Flawfinder: ignore */
			HRESULT hr = SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path);
			if(hr == NOERROR) {
				f_private_user_path = path;
				f_private_user_path = f_private_user_path.FilenameChild(f_manufacturer).FilenameChild(f_application_filename);
				if(append_version && !f_application_version.IsEmpty()) {
					f_private_user_path = f_private_user_path.FilenameChild(f_application_version);
				}
			}
			else {
				// it failed?! (should we throw?)
				f_private_user_path = "C:/";
			}
		}
		else {
			// do it a la Unix
			f_private_user_path = home.FilenameChild("." + f_application_filename);
			if(append_version && !f_application_version.IsEmpty()) {
				f_private_user_path += "-";
				f_private_user_path += f_application_version;
			}
		}
#else
		// Build the string: $HOME/.<appname>
		// Where the application name is transformed to lowercase and
		// spaces to underscores (_).
		// TODO: we need to remove "invalid" chars from the application
		//	 name (such as controls and !/?*.[]:; and so on.)
		moWCString home = getenv("HOME");	/* Flawfinder: ignore */
		f_private_user_path = home.FilenameChild("." + f_application_filename);
		if(append_version && !f_application_version.IsEmpty()) {
			f_private_user_path += "-";
			f_private_user_path += f_application_version;
		}
#endif
		// Make sure the directory has been created
		//
		moFile::CreateDir( f_private_user_path, 0700 );
	}

	return f_private_user_path;
}


/** \brief Reset the user path.
 *
 * If something changes, such as the name of the application or
 * the user logged in, then you may need to reset the user path
 * to be able to retrieve the correct path with GetUserPath().
 *
 * \sa GetUserPath
 */
void moApplication::ResetUserPath()
{
	f_user_path.Empty();
}


/** \brief Reset the private user path.
 *
 * If something changes, such as the name of the application or
 * the user logged in, then you may need to reset the user path
 * to be able to retrieve the correct path with GetUserPath().
 *
 * \sa GetPrivateUserPath
 */
void moApplication::ResetPrivateUserPath()
{
	f_private_user_path.Empty();
}


/** \brief Defines the manufacturer name.
 *
 * This function defines the name of the manufacturer of this
 * application.
 *
 * \sa GetPrivateUserPath, GetManufacturer
 */
void moApplication::SetManufacturer(const moWCString& name)
{
	f_manufacturer = name;
}


/** \brief Retrieve the name of the manufacturer.
 *
 * This function retrieves the name of the manufacturer.
 *
 * \sa GetPrivateUserPath, SetManufacturer
 */
const moWCString& moApplication::GetManufacturer() const
{
	return f_manufacturer;
}


/** \brief Set the command line options in the application.
 *
 * The application object is expected to take care of parsing the
 * command line options.
 *
 * Internally, the moApplication class handles the following
 * options:
 *
 * \li --config string
 * Accept a new name for the default configuration file.
 *
 * \li --app-name string
 * Rename the application
 *
 * \li --root-path string
 * The path to the auto-handled configuration and data files.
 *
 * \param[in] accept                The list of options your application accepts
 * \param[in] argc                  The number of command line arguements
 * \param[in] argv                  The array of arguements
 * \param[in] use_default_options   Whether the internal default options are to be used.
 */
void moApplication::SetOptions(const moWCString& accept, int argc, const char *argv[], bool use_default_options)
{
	f_getopt = new moGetOpt(accept + (use_default_options ?
		";"	// in case the user didn't end its string properly
		"[*];"	// all our options are optional
		"--config string;"
		"--app-name string;"
		"--root-path string;"
		: "")); // no default (internal) options
	f_getopt->SetVersion(f_application_version);
	f_getopt->SetCopyright(f_application_copyright);
	f_getopt->Main(argc, argv);

	moGetOpt::moOptionSPtr opt = Option("config");
	if(opt) {
		f_configuration_filename = opt->f_result;
	}
	opt = Option("app-name");
	if(opt) {
		SetName(opt->f_result);
	}
	opt = Option("root-path");
	if(opt) {
		SetRootPath(opt->f_result);
	}
}


/** \brief Retrieve the named option.
 *
 * After calling the SetOptions() function, you can retrieve your options
 * using this function.
 *
 * \param[in] name   The name of the option to retrieve
 *
 * \return A smart pointer to the option if it exists, NULL otherwise.
 */
moGetOpt::moOptionSPtr moApplication::Option(const moWCString& name) const
{
	if(f_getopt) {
		return f_getopt->Option(name);
	}

	return 0;
}


/** \brief Retrieve the identified option.
 *
 * After calling the SetOptions() function, you can retrieve your options
 * using this function.
 *
 * \param[in] identifier   The identifier of the option to retrieve
 *
 * \return A smart pointer to the option if it exists, NULL otherwise.
 */
moGetOpt::moOptionSPtr moApplication::Option(unsigned long identifier) const
{
	if(f_getopt) {
		return f_getopt->Option(identifier);
	}

	return 0;
}


/** \brief Retrieve the option array.
 *
 * This function directly retrieves the option array.
 *
 * \return A smart pointer to the option if it exists, NULL otherwise.
 */
moGetOptSPtr moApplication::GetOpt() const
{
	return const_cast<moGetOpt *>(static_cast<const moGetOpt *>(f_getopt));
}





/** \brief Retrive the application configuration file content.
 *
 * This function can be used to load the content of the configuration file
 * in the form or a property bag.
 *
 * The function takes care of the location and the name of the configuration
 * file.
 *
 * The name of the configuration file can be changed using the --config
 * command line option. Otherwise the application filename is used with
 * the .xml extension.
 *
 * The path is composed of the root path plus "etc".
 *
 * \return A property bag reference with the application configuration data.
 */
moPropBagRef moApplication::GetConfiguration()
{
	if(f_application_name.IsEmpty()) {
		throw moError("missing application name to be able to read the configuration file");
	}

	if(!f_configuration) {
		moWCString name;

		f_configuration.NewProp();
		if(f_configuration_filename.IsEmpty()) {
			// by default use app. name!
			name = f_application_filename + ".xml";
		}
		else {
			name = f_configuration_filename;
		}
		name = f_root_path.FilenameChild("etc").FilenameChild(name);
		moXMLLoadPropBag(name, f_configuration);
	}

	return f_configuration;
}


/** \brief Load a property bag from the specified file.
 *
 * This function loads a property bag from the file specified by name.
 *
 * If name includes a root path, then no path is prepended. If name
 * is relative, then the application path is prepended.
 *
 * If name has an extension, then none is added. Otherwise, this
 * function appends ".xml".
 *
 * Then load the file with moXMLLoadPropBag()
 *
 * This function is often used to load additional configuration data.
 *
 * \param[in] name      The name of the configuration file to read.
 * \param[in] prop_bag  The property bag where the data is to be loaded
 *
 * \return The result of the moXMLLoadPropBag() function, i.e. 0 when no error
 * occured, -1 if an error was detected.
 *
 * \sa GetConfiguration(), moXMLLoadPropBag()
 */
int moApplication::LoadPropBag(const moWCString& name, moPropBagRef prop_bag)
{
	if(name.IsEmpty()) {
		throw moError("invalid property bag name for moApplication:LoadPropBag()");
	}

	// we need to make a copy
	moWCString filename = name;

	// force an extension when none is specified
	if(!filename.FilenameHasExtension()) {
		filename += ".xml";
	}

	// if not an absolute path, prepend the application path
	if(filename[0] != mowc::wc_t('/')) {
		// the application path includes the root path
		filename = GetApplicationPath().FilenameChild(filename);
	}
	else {
		// always add the root path
		filename = f_root_path.FilenameChild(filename);
	}

	return moXMLLoadPropBag(filename, prop_bag);
}


/** \brief Run() until Quit is detected.
 *
 * The Run() function runs the application by calling the
 * event pipes at all time. This has the effect of
 * emptying the pipes and waiting for new events.
 *
 * Internally, we accept Broadcasted events.
 *
 * Externally, the user can declare an moWaitHandler
 * and register it with AddWaitHandler() (and later
 * remove it with RemoveWaitHandler()). Whenever the
 * application is idle (no more events are present
 * in the pipe), it will call the Wait() function
 * of the wait handlers.
 *
 * \bug
 * You cannot call the Run() function with a bare pointer to
 * the application. Please, use a smart pointer to save your
 * copy of the application object before to call the Run()
 * function.
 *
 * \sa Instance(), moWaitHandler, AddWaitHandler(), RemoveWaitHandler(), Wait(), DispatchAll()
 */
int moApplication::Run()
{
	moEventSPtr	event;

	while(f_run) {
		// execute our events
		while(Peek(event, true));

		// execute some user events (GTK, FLTK, network, etc.)
		CallWaitHandlers();
	}

	DispatchAll();

	// give a chance to the user to cleanup
	CallDoneHandlers();

	// Delete this instance -- we are done
	//
	Flush();
	Done();

	return 0;
}


/** \brief Dispatch all the events.
 *
 * The DispatchAll() function empties the event pipes. Once
 * idle, it calls the user Wait() function and then checks
 * the event pipes again. If still empty, the function
 * returns, otherwise the process is repeated.
 *
 * If you use the Run() function, the DispatchAll() function
 * will automatically be called for you.
 *
 * If the Run() function is not used, then the DispatchAll()
 * function does not call any Wait() function.
 *
 * This function only returns after all the events were
 * handled.
 *
 * \sa Wait(), Run()
 */
void moApplication::DispatchAll()
{
	bool		call_handlers;
	moEventSPtr	event;

	// already marked as not running?
	if(!f_run) {
		// we still want to execute our messages, but ignore
		// the wait handlers (for now... TBD)
		while(Peek(event, true));
		return;
	}

	call_handlers = true;

	// is there at least one of our events?
	while(Peek(event, true)) {
		// execute our events
		while(Peek(event, true));

		// execute some user events (GTK, FLTK, network, etc.)
		CallWaitHandlers();
		call_handlers = false;
	}

	if(call_handlers) {
		// call at least once, even if our pipe had no events
		CallWaitHandlers();
	}
}




/** \brief Add a wait handler.
 *
 * The AddWaitHandler() function will be used to add one or
 * more wait handler to the application. The same handler
 * can be added multiple times, though it is strongly suggested
 * that you do not do so.
 *
 * By default, the application will call the Wait() function
 * of the broadcast pipe. In effect, it will wait for an
 * internal event to be received.
 *
 * \note
 * Newly added handlers does not executed until the next time
 * the CallWaitHandlers() is called.
 *
 * \note
 * The handlers are not sorted automatically. Instead, they
 * are added one after another as the user defines them. The
 * handlers are called in the order the user defined them.
 * At this time there isn't a way to insert a new handler at
 * a specific position.
 *
 * \param[in] handler   The wait handler to add.
 *
 * \sa RemoveWaitHandler(), CallWaitHandlers()
 */
void moApplication::AddWaitHandler(const moWaitHandler& handler)
{
	moLockMutex lock(f_handlers_mutex);

	f_handlers += handler;
}


/** \brief Remove a wait handler that was added by AddWaitHandler()
 *
 * The RemoveWaitHandler() can be called to remove a handler
 * that was previously added with AddWaitHandler(). If the
 * same handler was added multiple times, you need to call
 * this function the same number of times to remove it completely.
 *
 * It is safe to call this function more times than the AddWaitHandler()
 * or even if the AddWaitHandler() was never called with that handler.
 *
 * \note
 * Newly removed handlers will still be executed until the current
 * CallWaitHandlers() returns.
 *
 * \param[in] handler   The wait handler to be removed from the application.
 *
 * \sa AddWaitHandler(), CallWaitHandlers()
 */
void moApplication::RemoveWaitHandler(const moWaitHandler& handler)
{
	moList::position_t	pos;

	moLockMutex lock(f_handlers_mutex);

	pos = f_handlers.Find(&handler);
	if(pos != moList::NO_POSITION) {
		f_handlers.Delete(pos);
	}
}


/** \brief Call all the wait handler Wait() function.
 *
 * The CallWaitHandlers() function is automatically called
 * by the Run() function whenever the application is idle.
 *
 * In order for the list of wait handlers to work in a
 * multi-threaded environment and to allow the use of the
 * AddWaitHandler() & RemoveWaitHandler() functions from
 * within a Wait() function, the current list of wait
 * handlers is copied before their Wait() functions are
 * called. While the copy is performed, the same mutex as
 * used to add and remove handlers is locked.
 *
 * \bug
 * (Is this statement still accurate?)
 * Your application will hang if this function is called
 * and no other thread is running and there is no wait
 * handler recorded.
 *
 * \sa Run(), AddWaitHandler(), RemoveWaitHandler(), Wait(), moWaitHandler::Wait()
 */
void moApplication::CallWaitHandlers()
{
	moList::position_t	idx, max;

	f_handlers_mutex.Lock();

	max = f_handlers.Count();
	if(max > 0UL) {
		if(max == 1UL) {
			// special case we can treat faster
			moWaitHandlerSPtr handler = f_handlers.Get(0);
			f_handlers_mutex.Unlock();
			handler->Wait();
		}
		else {
			// we want a copy so the Wait() functions can
			// call the Add & Remove functions (though they
			// probably shouldn't)
			moListOfWaitHandlers copy(f_handlers);
			f_handlers_mutex.Unlock();
			for(idx = 0; idx < max; ++idx) {
				moWaitHandlerSPtr handler = copy.Get(idx);
				handler->Wait();
			}
		}
	}
	else {
		f_handlers_mutex.Unlock();
		Wait();
	}
}

/** \brief Call all the Done() functions of the wait handlers.
 *
 * This function is called by the Run() function whenever the
 * Run() function is about to quit.
 *
 * It is expected that the Done() functions will call the RemoveWaitHandler()
 * although it is not required.
 *
 * The process in this function is the same as the CallWaitHandlers()
 * function.
 *
 * \sa moWaitHandler::Done(), AddWaitHandler(), RemoveWaitHandler(), CallWaitHandler()
 */
void moApplication::CallDoneHandlers()
{
	moList::position_t	idx, max;

	f_handlers_mutex.Lock();

	max = f_handlers.Count();
	if(max > 0UL) {
		if(max == 1UL) {
			// special case we can treat faster
			moWaitHandlerSPtr handler = f_handlers.Get(0);
			f_handlers_mutex.Unlock();
			handler->Done();
		}
		else {
			// we want a copy so the Wait() functions can
			// call the Add & Remove functions (though they
			// probably shouldn't)
			moListOfWaitHandlers copy(f_handlers);
			f_handlers_mutex.Unlock();
			for(idx = 0; idx < max; ++idx) {
				moWaitHandlerSPtr handler = copy.Get(idx);
				handler->Done();
			}
		}
	}
	else {
		f_handlers_mutex.Unlock();
	}
}




/** \brief Terminate the application
 *
 * The Terminate() function can either terminate
 * right away (i.e. force is set to true) or post a
 * message to all the receivers to request a
 * termination of the application.
 *
 * When the force flag is not set to true, a
 * receiver will usually check whether the user
 * really wants to quit (with a window such as:
 * "Do you really want to quit?" "Yes"/"No"),
 * and if so, that function will call Terminate()
 * again this time with the force flag set
 * to true.
 *
 * \param[in] force   Whether the termination should be forced or not.
 *
 * \sa Run(), moApplicationEvent class
 */
void moApplication::Terminate(bool force)
{
	if(force) {
		f_run = false;
	}
	else {
		moApplicationEvent::PostTerminate(this);
	}
}


/** \brief Clean up a wait handler.
 *
 * Cleans up a wait handler object.
 */
moApplication::moWaitHandler::~moWaitHandler()
{
}


/** \fn moWaitHandler::Wait()
 *
 * \brief The Wait() function of the wait handler.
 *
 * The Wait() function of registered moWaitHandler's
 * are called whenever the thread running the
 * moApplication::Run() function is idle.
 *
 * The moWaitHandler class needs to be derived for the
 * Wait() function to do something useful.
 */


/** \brief Function to handle the wait handler cleanup.
 *
 * The Done() function is called right before the Run() function
 * returns. It can clean up your objects as required.
 *
 * Although you can remove your wait handler from the
 * application using this function, it is not required
 * to do so.
 */
void moApplication::moWaitHandler::Done(void)
{
	// do nothing by default
}


/** \brief Compare two handlers by pointer.
 *
 * The default comparison function can be used to sort handlers by pointer.
 *
 * \param[in] object   The other wait handler to compare with this wait handler.
 */
moBase::compare_t moApplication::moWaitHandler::Compare(const moBase& object) const
{
	return ComparePtr(object);
}


/** \brief Clean up the applicatin event.
 *
 * Clean up the resources used by the application event object.
 *
 * \sa PostTerminate()
 */
moApplicationEvent::~moApplicationEvent()
{
}



/** \brief Post the terminate event
 *
 * The PostTerminate() function is used by the
 * moApplication::Terminate() function to request all
 * the receivers to terminate.
 *
 * This does not actually shutdown the application. It
 * only broadcast the termination signal to all the
 * objects. Calling moApplication::Terminate() with
 * true from one of these signal handler will really
 * terminate the application.
 *
 * \sa Terminate()
 */
void moApplicationEvent::PostTerminate(moEventPipeBroadcast *__broadcast)
{
/* Note: this was generated using async_functions */
	class __Event : public moReceiversEvent
	{
	public:
		__Event()
			: moReceiversEvent("Terminate")
		{
		}
		__Event(const __Event& event)
			: moReceiversEvent(event)
		{
		}
		const char *moGetClassName() const
		{
			return "molib::moBase::moEvent::__Event(Terminate)";
		}
		virtual moEventSPtr Duplicate() const
		{
			return new __Event(*this);
		}
		virtual void SendToReceivers(const moSortedList& receivers)
		{
			moList::position_t idx, max;
			moApplicationEvent *r;
			max = receivers.Count();
			for(idx = 0; idx < max; ++idx) {
				r = dynamic_cast<moApplicationEvent *>(receivers.Get(idx));
				if(r != 0) {
					r->Terminate();
				}
			}
		}
	} the_event;
	//
	__broadcast->Post(the_event);
}


/** \fn moApplicationEvent::Terminate()
 *
 * \brief Terminate the application.
 *
 * You implement this function in your objects if you want to receive the
 * Terminate signal. It is called whenever the Run() function is about
 * to quit.
 *
 * In a GUI environment, one of the Terminate() function will open a dialog
 * box to ask the user whether he really wanted to exit the application.
 *
 * \bug
 * This function must not be used to clean up. If you do that and have a
 * dialog that asks the user whether he wants to quit and he says no, you
 * will be in trouble.
 */


};		// namespace molib;
//
// vim: ts=8 sw=8 syntax=cpp.doxygen

