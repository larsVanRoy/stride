#############################################################################
#  This file is part of the Stride software. 
#  It is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by 
#  the Free Software Foundation, either version 3 of the License, or any 
#  later version.
#  The software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License,
#  along with the software. If not, see <http://www.gnu.org/licenses/>.
#  see http://www.gnu.org/licenses/.
#
#  Copyright 2017, 2018 Willem L, Kuylen E & Broeckhove J
#############################################################################

#============================================================================
# Add subdirs:
#============================================================================
if(NOT Protobuf_FOUND)
    add_subdirectory(protobuf)
endif()

if(NOT HDF5_FOUND)
       # Tell hdf5 that we are manually overriding certain settings
       set (HDF5_EXTERNALLY_CONFIGURED 1)
       set (BUILD_TESTING FALSE CACHE BOOL "Disable HDF5 build testing." FORCE)
       set (HDF5_BUILD_EXAMPLES FALSE CACHE BOOL "Do not build HDF5 examples." FORCE)
       set (HDF5_BUILD_TOOLS FALSE CACHE BOOL "Do not build HDF5 tools." FORCE)
       set (HDF5_EXPORTED_TARGETS "libstride")
       # Add the sub project
       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
       # To avoid tho "has no symbol warnings for empty .o's in static libs
       if(CMAKE_HOST_APPLE)
              SET(CMAKE_C_ARCHIVE_CREATE   "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
              SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
              SET(CMAKE_C_ARCHIVE_FINISH   "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
              SET(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
       endif()
       add_subdirectory (hdf5 EXCLUDE_FROM_ALL)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
add_subdirectory(sha1)
add_subdirectory(trng-4.20)

#############################################################################
