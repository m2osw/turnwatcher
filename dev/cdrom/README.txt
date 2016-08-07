Thank you for purchasing Turn Watcher (tm)! We at Made to Order Software
Corporation wish you many hours of quality game time!

If you have any installation issues or problems running the software, please
go to http://www.turnwatcher.com and click on "Support."


INSTALLATION INSTRUCTIONS:



WINDOWS 2000/XP/Vista:

1) Insert the cdrom into your computer's cdrom drive. After the disc spins up
   to speed, the setup program will launch automatically.

   If the Setup program does not start automatically, you can explore the CD
   and double click on the Setup file. This is the same as the automatic
   launch.

2) You will be prompted to read the license. After you have read the license,
   click "I accept the Turn Watcher license." With this button selected,
   you may press the "Next" button at the bottom right corner to continue
   the installation process.

3) The second page asks you what elements of the application you want to
   install. By default, the software and documentation are automatically
   selected. For instance, if you only want to install the software, uncheck
   the documentation; this way you might save some space on your hard disk.
   Please note, however, if you choose not to install the online
   documentation, you will not have access to it via the Help menu in the
   application. Once you are satisfied with your choice, click on Next to go to
   the last step.

4) The third page lets you choose the location where the software should be
   installed on your hard disk. Type in the location where you want to install
   the software, use the Browse button to select a new location, or leave it
   alone to accept the default location (this is recommended). You can type in
   a non-existant folder name in which case it will be created. Once you have
   chosen a destination folder, click on Install.

Once the installation is finished, the dialog shows a Close button at the
bottom right. Click on it to terminate the installation program.

You are ready to run Turn Watcher! Go to the Start menu, start the
application (Programs|Made to Order Software|Turn Watcher) and enjoy!



LINUX:

  o Debian Distributions

For Debian-based systems (such as Ubuntu, Kubuntu, and Debian), put the cdrom
disc in your drive and then follow the instructions below. Press ENTER after
each line beginning with "$" below (do NOT type the $):

1) Open a Terminal or Konsole session (this gives you a Linux shell prompt: $)

2) Mount your cdrom drive:

   $ mount /cdrom

3) Use the cd command to go into the cdrom directory:

   $ cd /cdrom

   Note: if you have an auto-mount, this command will return an error saying
         that the device is busy. This is normal.

4) Install the Turn Watcher package with the dpkg command:

   $ sudo dpkg -i turnwatcher_%FULL_VERSION%_i386.deb

5) If you see any errors about dependencies, then you should run the
   following command:

   $ sudo apt-get install -f 

6) Unmount your cdrom and then eject the disc.

   $ umount /cdrom
   $ eject /cdrom

7) Run Turn Watcher and enjoy!

   $ turnwatcher




  o RPM Distributions (RedHat or FedoraCore)

For RPM-based systems (such as RedHat and FedoraCore), put the cdrom disc in
your drive and then follow the instructions below. Press ENTER after each line
beginning with "$" below (do NOT type the $):

1) Open a Terminal or Konsole session (this gives you a Linux shell prompt: $)

2) Mount your cdrom drive:

   $ mount /cdrom

   Note: if you have an auto-mount, this command will return an error saying
         that the device is busy. This is normal.

3) Use the cd command to go into the cdrom directory:

   $ cd /cdrom

4) Install the Turn Watcher package with the rpm command:

   $ sudo rpm -i turnwatcher-%FULL_VERSION%.i386.rpm

5) If you see any errors about dependencies, then you should use the
   yum command to install them; then try point 4. Repeat point 4
   and 5 until it works.

   Please, refer to the documentation of the yum command for more
   information.

6) Unmount your cdrom and then eject the disc.

   $ umount /cdrom
   $ eject /cdrom

7) Run Turn Watcher and enjoy!

   $ turnwatcher



  o Other Linux Distributions

For other GNU/Linux distributions, put the cdrom disc in your drive and then
follow the instructions below. Press ENTER after each line beginning with "$"
below (do NOT type the $):

1) Open a Unix shell (Terminal or Console)

2) Mount your cdrom drive if you do not have auto-mount:

   $ mount /cdrom

   Note: if you have an auto-mount, this command will return an error saying
         that the device is busy. This is normal.

3) Change directory with the cd command into your home folder:

   $ cd ~

4) Extract the files:

   $ tar -xzf /cdrom/turnwatcher-%FULL_VERSION%_i386.tar.gz

   If your tar command doesn't understand the 'z' flag, use two commands like this:

   $ gunzip -c /cdrom/turnwatcher-%FULL_VERSION%_i386.tar.gz | tar xf -

5) Change directories into the turnwatcher folder.

   $ cd turnwatcher-%FULL_VERSION%

6) Run the install script:

   $ ./install.sh

7) If the script can't figure out the pathname of your browser, it will ask
   you for the full path of your current browser.

   Example: /usr/bin/firefox

8) Now you can unmount your cdrom and then eject the disc.

   $ umount /cdrom
   $ eject /cdrom

9) Run Turn Watcher and enjoy!

   $ ./turnwatcher


-The Turn Watcher Team

