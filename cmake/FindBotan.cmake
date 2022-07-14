# - Find botan
# Find the botan cryptographic library
#
# This module defines the following variables:
#   Botan_FOUND  -  True if library and include directory are found
# If set to TRUE, the following are also defined:
#   Botan_INCLUDE_DIRS  -  The directory where to find the header file
#   Botan_LIBRARIES  -  Where to find the library file
#
# For conveniance, these variables are also set. They have the same values
# than the variables above.  The user can thus choose his/her prefered way
# to write them.
#   Botan_LIBRARY
#   Botan_INCLUDE_DIR
#
# This file is in the public domain

find_package(PkgConfig REQUIRED)
pkg_check_modules(Botan botan-2)

if(NOT Botan_FOUND)
  find_path(Botan_INCLUDE_DIRS NAMES botan/botan.h
      PATH_SUFFIXES botan-2
      DOC "The botan include directory")

  find_library(Botan_LIBRARIES NAMES botan botan-2
      DOC "The botan library")

  # Use some standard module to handle the QUIETLY and REQUIRED arguments, and
  # set Botan_FOUND to TRUE if these two variables are set.
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(Botan REQUIRED_VARS Botan_LIBRARIES Botan_INCLUDE_DIRS)

  if(Botan_FOUND)
    set(Botan_LIBRARY ${Botan_LIBRARIES} CACHE INTERNAL "")
    set(Botan_INCLUDE_DIR ${Botan_INCLUDE_DIRS} CACHE INTERNAL "")
    set(Botan_FOUND ${Botan_FOUND} CACHE INTERNAL "")
  endif()
endif()

mark_as_advanced(Botan_INCLUDE_DIRS Botan_LIBRARIES)
