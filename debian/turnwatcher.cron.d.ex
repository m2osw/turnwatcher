#
# Regular cron jobs for the turnwatcher package
#
0 4	* * *	root	[ -x /usr/bin/turnwatcher_maintenance ] && /usr/bin/turnwatcher_maintenance
