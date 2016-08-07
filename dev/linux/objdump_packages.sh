SOLIBS=`objdump -p $1 | grep NEEDED | awk '{print $2}'`
PACKAGES=`dpkg -S ${SOLIBS} | awk '{print $1}' | sort -u | sed 's/\([^:]*\):/\1/'`
dpkg -l ${PACKAGES} | grep ^ii | awk '{printf("%s (>= %s), ", $2, $3)}'
