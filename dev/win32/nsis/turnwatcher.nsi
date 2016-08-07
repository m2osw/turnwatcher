# Installer information
#
# Find the makensis.exe file once you installed the files found
# here: http://nsis.sf.net
#
# Command line:		makensis TurnWatcher.nsi
# Result:		turnwatcher-install-<version>-win32-i686.exe
#
# Installer for TurnWatcher
# The installer version is saved as the last number in the VERSION
# string of the installer (see below)
# The version will need to be defined dynamically.
#
!define	INSTALLER_VERSION 2
Name "TurnWatcher @VERSION@-@BUILD_VERSION_STRING@"
Caption "Turn Watcher@DEMO@ @VERSION@-@BUILD_VERSION_STRING@ Installer"
OutFile @OUTPUT@
SetCompress auto
SetCompressor lzma
SetDateSave on
SetDatablockOptimize on
CRCCheck on
SilentInstall normal
XPStyle on
LicenseBkColor D8E2EC
BrandingText /trimcenter "Made to Order Software Corp."
SetFont Arial 8
AddBrandingImage top 67 0
InstallColors 000000 FFFFFF
# Don't put a \ after the version so that way the installer forces
# it after the path the user selects with the Browse button!
InstallDir "$PROGRAMFILES\Made to Order Software Corp\Turn Watcher@DEMO@\@VERSION@"
InstallDirRegKey HKLM "Software\Made to Order Software Corp.\Turn Watcher@DEMO@" "InstallDir"
InstType "Turn Watcher@DEMO@ (Full)"
InstType "Turn Watcher@DEMO@ only (Light)"
InstType "Documentation only (Docs)"
# START -- screensaver
InstType "Screensaver only"
# END -- screensaver


!define LANG_ENGLISH	1033
# Version 2.21 enforces the VIProductVersion to be X.X.X.X
#VIProductVersion "@VERSION@-@BUILD_VERSION_STRING@_${INSTALLER_VERSION}"
VIProductVersion "@VERSION@.@BUILD_VERSION@.${INSTALLER_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "@VERSION_MAJOR@, @VERSION_MINOR@, @BUILD_VERSION@, ${INSTALLER_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Turn Watcher@DEMO@"
VIAddVersionKey /LANG=${LANG_ENGLISH} "InternalName" "Turn Watcher@DEMO@"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "What all the dungeons masters have been waiting for! An automated round tracker."
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Made to Order Software Corp."
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© 2005-2006 by Made to Order Software Corp."
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "Turn Watcher is a legal trademark by Made to Order Software Corporation"
VIAddVersionKey /LANG=${LANG_ENGLISH} "OriginalFilename" "@OUTPUT@"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Turn Watcher@DEMO@ Installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "@VERSION@.@BUILD_VERSION@.${INSTALLER_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "PrivateBuild" "Selling a d20 game? Want to bundle Turn Watcher with it? Contact Made to Order Software Corp. http://www.m2osw.com"
VIAddVersionKey /LANG=${LANG_ENGLISH} "SpecialBuild" "We have versions for CD and Internet installations"

Function .onInit
	Initpluginsdir
	SetOutPath "$TEMP"
	File "turnwatcher-install.bmp"
FunctionEnd

Function .onGUIInit
	SetBrandingImage "$TEMP\turnwatcher-install.bmp"
FunctionEnd

Function un.onInit
	#Initpluginsdir
	SetOutPath "$TEMP"
	File "turnwatcher-install.bmp"
FunctionEnd

Function un.onGUIInit
	SetBrandingImage "$TEMP\turnwatcher-install.bmp"
FunctionEnd



# Pages for installation
PageEx license
	LicenseData "share\turnwatcher\doc\LICENSE.rtf"
	LicenseText "Turn Watcher@DEMO@ License"
	LicenseForceSelection radiobuttons "I accept the Turn Watcher@DEMO@ license" "I refuse the Turn Watcher@DEMO@ license"
PageExEnd

PageEx components
	ComponentText "Turn Watcher@DEMO@"
PageExEnd

PageEx directory

# Add option of "start as soon as it is installed"
# Look here for how to add a custom button (use the ioC.ini file example)
# C:\Program Files\NSIS\Examples\Modern UI

PageExEnd

PageEx instfiles
PageExEnd

# Pages for uninstalling
UninstPage uninstConfirm
UninstPage instfiles


# Sections - where files to be installed are listed
Section "-base"
	SectionIn 1 2 3
	SetDetailsPrint both
	DetailPrint "Installing the Turn Watcher@DEMO@ uninstaller"
	SetDetailsPrint listonly
	SetOutPath "$INSTDIR\bin"
	WriteUninstaller "bin\uninstall-turnwatcher-@VERSION@.exe"

	WriteRegStr		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "UninstallString" '"$INSTDIR\bin\uninstall-turnwatcher-@VERSION@.exe"'
	WriteRegStr		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "InstallLocation" "$INSTDIR"
	WriteRegStr		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "DisplayName" "Turn Watcher"
	WriteRegStr		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "DisplayIcon" "$INSTDIR\bin\TurnWatcher.exe"
	WriteRegStr		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "DisplayVersion" "@VERSION@-@BUILD_VERSION_STRING@"
	WriteRegDWORD		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "VersionMajor" "@VERSION_MAJOR@"
	WriteRegDWORD		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "VersionMinor" "@VERSION_MINOR@"
	WriteRegDWORD		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "VersionBuild" "@BUILD_VERSION@"
	WriteRegDWORD		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "VersionInstaller" "${INSTALLER_VERSION}"
	WriteRegStr		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "URLInfoAbout" "http://www.turnwatcher.com/"
	WriteRegDWORD		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "NoModify" "1"
	WriteRegDWORD		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "NoRepair" "1"
	WriteRegStr		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "HelpLink" "http://www.turnwatcher.com/documentation"
	WriteRegStr		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "HelpTelephone" "+1 (916) 220-6482"
	WriteRegStr		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "RegOwner" "Made to Order Software Corporation"
	WriteRegStr		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "RegCompany" "Made to Order Software Corporation"
	WriteRegStr		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "Publisher" "Made to Order Software Corporation"
	WriteRegStr		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "URLUpdateInfo" "http://www.turnwatcher.com"
	WriteRegStr		HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "ProductID" "turnwatcher1"

	SetDetailsPrint both
	DetailPrint "Installing Turn Watcher@DEMO@ data"
	SetDetailsPrint listonly

	WriteRegStr		HKCR ".turnwatcher1" "" "TurnWatcher.turnwatcher1"
	WriteRegStr		HKCR ".turnwatcher1" "ContentType" "text/xml"
	#WriteRegStr		HKCR ".tw" "" "TurnWatcher.turnwatcher1"
	WriteRegStr		HKCR "TurnWatcher.turnwatcher1" "" "Turn Watcher Characters and Monsters file"
	WriteRegStr		HKCR "TurnWatcher.turnwatcher1\DefaultIcon" "" "$INSTDIR\bin\TurnWatcher.exe,0"
	WriteRegStr		HKCR "TurnWatcher.turnwatcher1\Shell" "" "Open"
	WriteRegStr		HKCR "TurnWatcher.turnwatcher1\Shell\Open" "" "&Open"
	WriteRegStr		HKCR "TurnWatcher.turnwatcher1\Shell\Open\Command" "" '"$INSTDIR\bin\TurnWatcher.exe" "%1"'
	WriteRegStr		HKCR "TurnWatcher" "" "Turn Watcher (the initiative manager)"
	WriteRegStr		HKCR "TurnWatcher\Shell" "" "Open"
	WriteRegStr		HKCR "TurnWatcher\Shell\Open" "" "&Open"
	WriteRegStr		HKCR "TurnWatcher\Shell\Open\Command" "" '"$INSTDIR\bin\TurnWatcher.exe" "%1"'

	SetOutPath "$INSTDIR"
	FileOpen $0 "Turn Watcher.url" "w"
	FileWrite $0 "[InternetShortcut]"
	FileWriteByte $0 "13"
	FileWriteByte $0 "10"
	FileWrite $0 "URL=http://www.turnwatcher.com"
	FileWriteByte $0 "13"
	FileWriteByte $0 "10"
	# Once we have a URL icon
	#FileWrite $0 "IconIndex=1"
	#FileWrite $0 "IconFile=$INSTDIR\bin\TurnWatcher.exe"
	FileClose $0

	SetOutPath "$INSTDIR\share\turnwatcher\doc"
	File share\turnwatcher\doc\LICENSE.txt
	File share\turnwatcher\doc\LICENSE.rtf

	CreateDirectory "$SMPROGRAMS\Made to Order Software"
	CreateShortCut "$SMPROGRAMS\Made to Order Software\Uninstall Turn Watcher@DEMO@.lnk" "$INSTDIR\bin\uninstall-turnwatcher-@VERSION@.exe" "" "" 0 SW_SHOWNORMAL "" "Remove all the Turn Watcher@DEMO@ files from your computer"
SectionEnd

Section "Turn Watcher@DEMO@"
	SectionIn 1 2

	SetDetailsPrint both
	DetailPrint "Installing Turn Watcher@DEMO@"
	SetDetailsPrint listonly

	SetOutPath "$INSTDIR"
	File /r bin
	File /r lib
	File /r share

	# This is transformed by our shell script
	SetOutPath "$INSTDIR\etc\gtk-2.0"
	File "etc\gtk-2.0\gtkrc"

	# This may only be for glade, but that's tiny
	#SetOutPath "$INSTDIR\etc"
	#File /r "etc\fonts"

	CreateDirectory "$SMPROGRAMS\Made to Order Software"
	CreateShortCut "$SMPROGRAMS\Made to Order Software\Read Turn Watcher License.lnk" "$INSTDIR\share\turnwatcher\doc\LICENSE.txt" "" "" 0 SW_SHOWNORMAL "" "The license of Turn Watcher@DEMO@"
	CreateShortCut "$SMPROGRAMS\Made to Order Software\Turn Watcher@DEMO@.lnk" "$INSTDIR\bin\TurnWatcher.exe" "" "" 0 SW_SHOWNORMAL "" "Turn Watcher@DEMO@ -- the tool for the Dungeon Masters"
SectionEnd

Section "Turn Watcher@DEMO@ Documentation"
	SectionIn 1 3
	SetDetailsPrint both
	DetailPrint "Installing the documentation"
	SetDetailsPrint listonly
	SetOutPath "$INSTDIR"
	File /r doc

	CreateShortCut "$SMPROGRAMS\Made to Order Software\Read Turn Watcher License.lnk" "$INSTDIR\share\turnwatcher\doc\en\LICENSE.html" "" "" 0 SW_SHOWNORMAL "" "The license of Turn Watcher@DEMO@"
	CreateShortCut "$SMPROGRAMS\Made to Order Software\Turn Watcher@DEMO@ Help.lnk" "$INSTDIR\share\turnwatcher\doc\en\index.html" "" "" 0 SW_SHOWNORMAL "" "All the help you need about Turn Watcher@DEMO@"
SectionEnd

Section "Turn Watcher@DEMO@ desktop icon"
	SectionIn 1 2
	SetDetailsPrint both
	DetailPrint "Installing the desktop icon"
	SetDetailsPrint listonly

	# Install the shortcut only if the executable is available
	IfFileExists "$INSTDIR\bin\TurnWatcher.exe" 0 +2
		CreateShortCut "$DESKTOP\Turn Watcher@DEMO@.lnk" "$INSTDIR\bin\TurnWatcher.exe" "" "" 0 SW_SHOWNORMAL "" "Turn Watcher@DEMO@ -- the tool for the Dungeon Masters"
SectionEnd

# START -- screensaver
Section "Turn Watcher Screensaver"
	SectionIn 4
	SetDetailsPrint both
	DetailPrint "Installing the screensaver"
	SetDetailsPrint listonly

	SetOutPath "$TEMP"
	File "turnwatcher-screensaver.exe"
	ExecWait '"$TEMP\turnwatcher-screensaver.exe" -f'
SectionEnd
# END -- screensaver

Section "-post"
	SectionIn 1 2 3
	SetDetailsPrint listonly
	WriteRegStr HKLM "Software\Made to Order Software Corp.\Turn Watcher@DEMO@" "InstallDir" "$INSTDIR"
	WriteRegStr HKLM "Software\Made to Order Software Corp.\Turn Watcher@DEMO@" "Version" "@VERSION@.@BUILD_VERSION@"

	# Execute the GDK query commands so Turn Watcher actually works
	SetOutPath "$INSTDIR"

	CreateDirectory "$INSTDIR\etc\pango"
	CreateDirectory "$INSTDIR\etc\gtk-2.0"

	IfFileExists "$INSTDIR\bin\tw-setup.exe" 0 +2
		ExecWait '"$INSTDIR\bin\tw-setup.exe" $INSTDIR'

	# Using the system.dll plugin to call the SHChangeNotify Win32 API
	# function so we can update the shell icons [doesn't seem functional]
	System::Call "shell32::SHChangeNotify(i,i,i,i) (0x08000000, 0x1000, 0, 0)"

	SetDetailsPrint both
	DetailPrint "Installation of Turn Watcher@DEMO@ completed."
	SetDetailsPrint listonly
SectionEnd

# The following will delete what this installer
# installs (especially the registry keys!)
# DO NOT CHANGE THE NAME OF THIS SECTION
Section "Uninstall"
	SetDetailsPrint both
	DetailPrint "Uninstalling Turn Watcher@DEMO@"
	SetDetailsPrint listonly

	ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@" "InstallLocation"

	# Instead of this message box I added the uninstConfirm page
	#MessageBox MB_YESNO "The Turn Watcher uninstaller is ready to remove Turn Watcher from your computer.$\r$\nIf you created files in the Turn Watcher folders, they will also be removed.$\r$\nAre you sure you want to remove Turn Watcher? (We strongly recommand you don't)" IDYES confirmed
	#Abort "Uninstallation of Turn Watcher Cancelled"
	#confirmed:

	IfFileExists $INSTDIR\TurnWatcher.exe turnwatcher_is_installed
		MessageBox MB_YESNO "Turn Watcher doesn't seem to be installed in '$INSTDIR\TurnWatcher.exe'.$\r$\nAre you sure you want to continue?" IDYES turnwatcher_is_installed
		Abort "Uninstallation of Turn Watcher cancelled"
	turnwatcher_is_installed:

	# We delete the whole folder even if the user created stuff in there!
	RMDir /r /REBOOTOK $0
	Delete "$SMPROGRAMS\Made to Order Software\Turn Watcher@DEMO@.lnk"
	Delete "$DESKTOP\Turn Watcher@DEMO@.lnk"
	Delete "$SMPROGRAMS\Made to Order Software\Read Turn Watcher License.lnk"
	Delete "$SMPROGRAMS\Made to Order Software\Turn Watcher@DEMO@ Help.lnk"
	Delete "$SMPROGRAMS\Made to Order Software\Uninstall Turn Watcher@DEMO@.lnk"
	# Remove the menu ONLY if it is empty (no other software installed in there)
	RMDir "$SMPROGRAMS\Made to Order Software"
	# We leave the Turn Watcher info since we'd like to use it on next install
	#DeleteRegKey HKLM "Software\Made to Order Software Corp.\Turn Watcher@DEMO@"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Turn Watcher@DEMO@"

	# We assume that a different version is not already installed...
	# TODO: test if other version(s) installed, do not delete classes
	DeleteRegKey HKCR ".turnwatcher"
	DeleteRegKey HKCR ".tw"
	DeleteRegKey HKCR "TurnWatcher"
	DeleteRegKey HKCR "TurnWatcher.turnwatcher"
SectionEnd

