##===============================================================================
## Copyright (c) 2005-2013 by Made to Order Software Corporation
## 
## All Rights Reserved.
## 
## The source code in this file ("Source Code") is provided by Made to Order Software Corporation
## to you under the terms of the GNU General Public License, version 2.0
## ("GPL").  Terms of the GPL can be found in doc/GPL-license.txt in this distribution.
## 
## By copying, modifying or distributing this software, you acknowledge
## that you have read and understood your obligations described above,
## and agree to abide by those obligations.
## 
## ALL SOURCE CODE IN THIS DISTRIBUTION IS PROVIDED "AS IS." THE AUTHOR MAKES NO
## WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
## COMPLETENESS OR PERFORMANCE.
##===============================================================================

#set( turnwatcher_PACKAGE_NAME  "${PROJECT_NAME}" )
#set( turnwatcher_VERSION_MAJOR 2                 )
#set( turnwatcher_VERSION_MINOR 0                 )
#set( turnwatcher_VERSION_PATCH 0                 )

#add_subdirectory( convert )
add_subdirectory( molib   )
#add_subdirectory( motk    )
add_subdirectory( gui     )
#add_subdirectory( package )

# vim: ts=4 sw=4 noexpandtab
