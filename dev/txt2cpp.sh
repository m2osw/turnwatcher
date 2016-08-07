#!/bin/sh
#
# File:		$Source: /mnt/cvs/root/m2osw/development/commercial/turnwatcher/mainline/txt2cpp.sh,v $
# Object:	Converts a text file to a compilable CPP file
# Version:	$Revision: 1.2 $
# Author:	$Author: alexis $
# Edited by:	$Locker:  $
# Last Edit:	$Date: 2005/12/24 05:06:05 $
# Copyright:	Copyright (c) 2005 Made to Order Software Corp.
#		All Rights Reserved.
#
#		This software and its associated documentation contains
#		proprietary, confidential and trade secret information
#		of Made to Order Software Corp. and except as provided by
#		written agreement with Made to Order Software Corp.
#
#		a) no part may be disclosed, distributed, reproduced,
#		   transmitted, transcribed, stored in a retrieval system,
#		   adapted or translated in any form or by any means
#		   electronic, mechanical, magnetic, optical, chemical,
#		   manual or otherwise,
#
#		and
#
#		b) the recipient is not entitled to discover through reverse
#		   engineering or reverse compiling or other such techniques
#		   or processes the trade secrets contained therein or in the
#		   documentation.
#
# Log:
# $Log: txt2cpp.sh,v $
# Revision 1.2  2005/12/24 05:06:05  alexis
# Added a space at the end of all the lines. This makes it look a bit better
# in the About -> License window. Also, totally empty lines are removed whereas
# a line with just a space stays.
#
# Revision 1.1  2005/12/13 01:00:52  alexis
# A script to transform the text license to a C++ file.
#
#
#

INPUT=doc/LICENSE.txt
OUTPUT=src/license.cpp

echo "/* auto-generated file, DO NOT EDIT */" >$OUTPUT
echo "static const char product_license[] = {" >>$OUTPUT


sed -e 's/\t/        /g' \
	-e 's/"/\\"/g' \
	-e 's/.*/	"& \\n"/' \
	$INPUT >>$OUTPUT

	#-e 's/"\\n"/" \\n"/' \


echo "};" >>$OUTPUT
