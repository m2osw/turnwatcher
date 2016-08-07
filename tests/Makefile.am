#!/bin/sh
#
# File:		Makefile.am
# Object:	Makefile for the tests directory
#
# Copyright:	Copyright (c) 2005-2007 Made to Order Software Corp.
#
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

bin_PROGRAMS	= StatTest

StatTest_SOURCES = StatTest.cpp

# set the include path found by configure
INCLUDES = $(all_includes)
SUFFIXES=.S .c .cpp .c++ .lo .o .obj .s

turnwatcher_LDFLAGS = $(all_libraries) $(LDFLAGS)

#
# vim: ts=8 sw=8 ai
