#
# File:		m2osw/source/sandbox/mo/<version>/generators/Makefile.am
# Object:	Description to know how to build all the generators
#
# Copyright:	Copyright (c) 2005-2008 Made to Order Software Corp.
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


# not 100% a GNU package
AUTOMAKE_OPTIONS = foreign 1.4

# Here we make sure that we compile for this running system instead of
# the destination (i.e. in case we're cross compiling)
CXX = @CXX_FOR_BUILD@
CPPFLAGS = 
CXXFLAGS = -O2
LDFLAGS = 
EXEEXT =

bin_PROGRAMS = controlled_vars async_functions transaction_builder

controlled_vars_SOURCES = controlled_vars.c++

async_functions_SOURCES = async_functions.c++

transaction_builder_SOURCES = transaction_builder.c++

