#
# SPDX-License-Identifier: BSD-3-Clause
#
# QUAZIP_FOUND               - QuaZip library was found
# QUAZIP_INCLUDE_DIR         - Path to QuaZip include dir
# QUAZIP_INCLUDE_DIRS        - Path to QuaZip and zlib include dir (combined from QUAZIP_INCLUDE_DIR + ZLIB_INCLUDE_DIR)
# QUAZIP_LIBRARIES           - List of QuaZip libraries
# QUAZIP_ZLIB_INCLUDE_DIR    - The include dir of zlib headers


if(Qt5Core_FOUND)
  set(QUAZIP_LIB_VERSION_SUFFIX 5)
elseif(Qt6Core_FOUND)
  set(QUAZIP_LIB_VERSION_SUFFIX 6)
endif(Qt5Core_FOUND)

find_package(QuaZip-Qt${QUAZIP_LIB_VERSION_SUFFIX} QUIET)

if(QuaZip-Qt${QUAZIP_LIB_VERSION_SUFFIX}_FOUND)
  set(QUAZIP_INCLUDE_DIRS QuaZip::QuaZip)
  set(QUAZIP_LIBRARIES QuaZip::QuaZip)
  set(QUAZIP_FOUND TRUE)
elseif(QUAZIP_INCLUDE_DIRS AND QUAZIP_LIBRARIES)
  set(QUAZIP_FOUND TRUE)
else()
  find_library(QUAZIP_LIBRARIES
    NAMES quazip${QUAZIP_LIB_VERSION_SUFFIX}
    HINTS /usr/lib /usr/lib64
  )

  find_path(QUAZIP_INCLUDE_DIR quazip.h
    HINTS /usr/include /usr/local/include
    PATH_SUFFIXES quazip${QUAZIP_LIB_VERSION_SUFFIX}
  )

  find_path(QUAZIP_ZLIB_INCLUDE_DIR zlib.h
    HINTS /usr/include /usr/local/include
  )

  set(QUAZIP_INCLUDE_DIRS ${QUAZIP_INCLUDE_DIR} ${QUAZIP_ZLIB_INCLUDE_DIR})

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(QuaZip DEFAULT_MSG
    QUAZIP_LIBRARIES
    QUAZIP_INCLUDE_DIR
    QUAZIP_ZLIB_INCLUDE_DIR
    QUAZIP_INCLUDE_DIRS
  )
endif()
