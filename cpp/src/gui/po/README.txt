
For more info see:

	http://www.gnu.org/software/gettext/manual/html_chapter/gettext.html

Creating the .mo files by hand (This should be automatable using automake?!)

	# Create a Template PO file
	xgettext -o turnwatcher.pot *.cpp *.h

	# Create a po directory & the languages
	mkdir -p po/{fr,es}/LC_MESSAGES
	cd po

	# Initialize PO files
	msginit --input ../turnwatcher.pot -l fr
	msginit --input ../turnwatcher.pot -l es

	# Translate the files... if you can

# The following is to test right here, with a make install
# it should go at the right place (except for Windows of
# course... but Alexis' will manage)

	# Compile to a .mo
	msgfmt.exe -o fr/LC_MESSAGES/turnwatcher.mo fr.po
	msgfmt.exe -o es/LC_MESSAGES/turnwatcher.mo es.po

