#!/bin/sh
#

# We do not need the libs links anymore
# AW, Aug 27, 2006
exit 1

LIBS="libatk-1.0.so.0.1010.3 \
	libatkmm-1.6.so.1.0.23 \
	libcairo.so.2.2.3 \
	libgdkmm-2.4.so.1.0.23 \
	libgdk_pixbuf-2.0.so.0.800.6 \
	libgdk_pixbuf_xlib-2.0.so.0.800.6 \
	libgdk-x11-2.0.so.0.800.6 \
	libglib-2.0.so.0.800.3 \
	libglibmm-2.4.so.1.0.16 \
	libglibmm_generate_extra_defs-2.4.so.1.0.16 \
	libgmodule-2.0.so.0.800.3 \
	libgobject-2.0.so.0.800.3 \
	libgthread-2.0.so.0.800.3 \
	libgtkmm-2.4.so.1.0.23 \
	libgtk-x11-2.0.so.0.800.6 \
	libpango-1.0.so.0.1001.0 \
	libpangocairo-1.0.so.0.1001.0 \
	libpangoft2-1.0.so.0.1001.0 \
	libpangomm-1.4.so.1.0.23 \
	libpangox-1.0.so.0.1001.0 \
	libpangoxft-1.0.so.0.1001.0 \
	libsigc-2.0.so.0.0.0 \
	libjpeg.so.62.0.0 \
	libpng12.so.0.1.2.8 \
	libtiff.so.4.0.0 \
	libz.so.1.2.2"

unset VERBOSE

if [ "$1" == "-v" ]
then
	VERBOSE=true
	shift
fi

if test -n "$1"
then
	if test -n "${VERBOSE}"; then echo "Changing into directory \"$1\""; fi
	cd $1
fi


for i in ${LIBS}
do
	shortname=`echo $i | sed 's/\(.*\)\.so\.\([^\.]*\).*/\1.so.\2/'`
	if test -n "${VERBOSE}"; then echo "Linking $i --> $shortname"; fi
	ln -fs $i $shortname
done
