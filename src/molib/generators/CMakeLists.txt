##===============================================================================
## Copyright (c) 2005-2017 by Made to Order Software Corporation
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

########### next target ###############
project( controlled_vars )

SET(controlled_vars_SRCS
   controlled_vars.cpp
)
add_executable(${PROJECT_NAME} ${controlled_vars_SRCS})
target_link_libraries(controlled_vars)

#install(TARGETS controlled_vars DESTINATION bin)


########### next target ###############
project( async_functions )

SET(async_functions_SRCS
   async_functions.cpp
)

add_executable(${PROJECT_NAME} ${async_functions_SRCS})

target_link_libraries(${PROJECT_NAME})

#install(TARGETS ${PROJECT_NAME} DESTINATION bin)


########### next target ###############
project( transaction_builder )

SET(transaction_builder_SRCS
   transaction_builder.cpp
)

add_executable(${PROJECT_NAME} ${transaction_builder_SRCS})

target_link_libraries(${PROJECT_NAME})

#install(TARGETS ${PROJECT_NAME} DESTINATION bin)


# vim: ts=4 sw=4 noexpandtab
