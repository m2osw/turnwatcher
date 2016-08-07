#!/bin/sh
#
# File:		mainline/src/Makefile.am
# Object:	Makefile for the source directory
#
# Copyright:	Copyright (c) 2005-2006 Made to Order Software Corp.
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




bin_PROGRAMS	= character-maint

character_maint_SOURCES = \
	character-maint.cpp

TW_OBJS = \
	$(srcdir)/../AboutWindow.o \
	$(srcdir)/../ActionsManager.o \
	$(srcdir)/../AppSettings.o \
	$(srcdir)/../CharacterColumns.o \
	$(srcdir)/../CharacterListUI.o \
	$(srcdir)/../CharacterManager.o \
	$(srcdir)/../character.o \
	$(srcdir)/../common.o \
	$(srcdir)/../DamageWindow.o \
	$(srcdir)/../DefineInitWindow.o \
	$(srcdir)/../DuplicateResolver.o \
	$(srcdir)/../DuplicateRoll.o \
	$(srcdir)/../DuplicateWindow.o \
	$(srcdir)/../EditWindow.o \
	$(srcdir)/../EffectEditor.o \
	$(srcdir)/../EffectHandler.o \
	$(srcdir)/../effect.o \
	$(srcdir)/../EffectsBook.o \
	$(srcdir)/../EffectsEditor.o \
	$(srcdir)/../EffectsList.o \
	$(srcdir)/../ExpiredEffectsWindow.o \
	$(srcdir)/../InfoBox.o \
	$(srcdir)/../InfoWindow.o \
	$(srcdir)/../InitiativeBase.o \
	$(srcdir)/../InitiativeManager.o \
	$(srcdir)/../InitWindow.o \
	$(srcdir)/../InputWindow.o \
	$(srcdir)/../JumpInWindow.o \
	$(srcdir)/../legacy_app.o \
	$(srcdir)/../legacy_character.o \
	$(srcdir)/../license.o \
	$(srcdir)/../MainWindow.o \
	$(srcdir)/../NagWindow.o \
	$(srcdir)/../SingletonManager.o \
	$(srcdir)/../SoftColumns.o \
	$(srcdir)/../splash.o \
	$(srcdir)/../stat.o \
	$(srcdir)/../StatManager.o \
	$(srcdir)/../transaction.o \
	$(srcdir)/../BleedOutTransaction.o \
	$(srcdir)/../CharacterEntry.o \
	$(srcdir)/../DamageTransaction.o \
	$(srcdir)/../DelayTransaction.o \
	$(srcdir)/../ForceInitiative.o \
	$(srcdir)/../MoveCharacterInInitiative.o \
	$(srcdir)/../MoveSelectedCharacters.o \
	$(srcdir)/../NextInitTransaction.o \
	$(srcdir)/../ResolveInitiativeDuplicates.o \
	$(srcdir)/../RollTransaction.o \
	$(srcdir)/../SetDefaultSort.o \
	$(srcdir)/../SortTransaction.o \
	$(srcdir)/../StabilizeTransaction.o \
	$(srcdir)/../UITransactionBase.o

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
if !WIN32
AM_CPPFLAGS = -DDATADIR="\"${datadir}\""
endif

character_maint_LDFLAGS = $(all_libraries) $(LDFLAGS) $(TW_OBJS)

# vim: ts=8 sw=8 ai
