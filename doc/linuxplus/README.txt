
We are glad you have decided to use Turn Watcher(tm) created by
Made to Order Software Corporation in 2006.

You can find the official website at:   http://www.turnwatcher.com
Our website is available in:

	English
	Espanol
	Francais

Full installation instructions are available here:

		http://www.turnwatcher.com/install_linux.html

The following are the installation instructions for Debian-based
Systems and other Linux.

  + Debian-based Systems
    (Ubuntu, Debian, Xandros, etc.)

    For Debian-based systems, download the Debian package and then
    follow the instructions below:

    * Open a Terminal or Konsole session (this gives you a Linux
      shell prompt: $)

    * Use the cd command to go to the directory in which you
      downloaded Turn Watcher.

      Example:
      $ cd ~/Desktop/Downloads

    * Install the Turn Watcher package with the dpkg command:

      $ sudo dpkg -i turnwatcher_1.1-m2osw7_i386.deb

    * If you see any errors about dependencies, then you should run
      the following command:

      $ sudo apt-get install -f

    * If you have Gnome or the KDE, Turn Watcher now appears in
      your Games menu.



  + Other Linux Distributions

    For other GNU/Linux distributions, download the tarball. You
    get all the files in that one tarball. Follow the instructions
    below:

    * Download the file in the directory where you want to install
      Turn Watcher. We suggest that you name that directory
      TurnWatcher.

    * Open a Unix shell (Terminal or Console)

    * Use the cd command to go to the directory in which you downloaded
      Turn Watcher.

      Example:
      $ cd ~/TurnWatcher

    * Extract the files:

      $ tar -xzf turnwatcher-1.1-m2osw7_i386.tar.gz

    * If your tar command doesn't understand the 'z' flag, use two commands
      like this:

      $ gunzip turnwatcher-1.1-m2osw7_i386.tar.gz
      $ tar -xf turnwatcher-1.1-m2osw7_i386.tar

    * Change directories into the turnwatcher folder.

      $ cd turnwatcher-1.1-m2osw7_i386

    * Run the install script:

      $ ./install.sh

    * If the script can't figure out the pathname of your browser, it will
      ask you for the full path of your current browser.

      Example: /usr/bin/firefox

    * Once finished, you can then run Turn Watcher!

      $ ./turnwatcher


