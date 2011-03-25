# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

PROJECT(lgpc C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE(dist.cmake)

# We need GPC to compile
FIND_PACKAGE(GPC REQUIRED)
INCLUDE_DIRECTORIES(${GPC_INCLUDE_DIRS})

# Build
ADD_LUA_MODULE(gpc lgpc.c gpc.def)
TARGET_LINK_LIBRARIES(gpc ${GPC_LIBRARIES})

# Install all files and documentation
INSTALL (TARGETS gpc DESTINATION ${INSTALL_CMOD})
INSTALL (FILES README DESTINATION ${INSTALL_DATA})
INSTALL (FILES test.lua DESTINATION ${INSTALL_TEST})
