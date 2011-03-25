# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec <kapecp@gmail.com>
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
#	Note:
#		Searching headers and libraries is very simple and is NOT as powerful as scripts
#		distributed with CMake, because LuaDist defines directories to search for.
#		Everyone is encouraged to contact the author with improvements. Maybe this file
#		becomes part of CMake distribution sometimes.

# - Find GPC
# Find the native GPC headers and libraries.
#
# GPC_INCLUDE_DIRS	- where to find gpc.h.
# GPC_LIBRARIES	- List of libraries when using GPC.
# GPC_FOUND	- True if GPC found.

# Look for the header file.
FIND_PATH(GPC_INCLUDE_DIR NAMES gpc.h
  PATH_SUFFIXES gpcl # debian
)

# Look for the library.
FIND_LIBRARY(GPC_LIBRARY NAMES gpc libgpc
  gpcl # debian
)

# Handle the QUIETLY and REQUIRED arguments and set GPC_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GPC DEFAULT_MSG GPC_LIBRARY GPC_INCLUDE_DIR)

# Copy the results to the output variables.
IF(GPC_FOUND)
	SET(GPC_LIBRARIES ${GPC_LIBRARY})
	SET(GPC_INCLUDE_DIRS ${GPC_INCLUDE_DIR})
ELSE(GPC_FOUND)
	SET(GPC_LIBRARIES)
	SET(GPC_INCLUDE_DIRS)
ENDIF(GPC_FOUND)

MARK_AS_ADVANCED(GPC_INCLUDE_DIRS GPC_LIBRARIES)
