cmake_minimum_required(VERSION 3.3)

project(CoreCtrl LANGUAGES CXX)
set(PROJECT_VERSION 1.2.0-dev)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# add uninstall target
if(NOT TARGET uninstall)
 configure_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
   "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
   IMMEDIATE @ONLY)

 add_custom_target(uninstall
   COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

list(APPEND TARGET_COMMON_COMPILE_FEATURES
  cxx_std_17              # C++17
)

option (WITH_DEBUG_INFO "Add debug information (to Debug builds)" OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions("-D_DEBUG")
  add_compile_options("-W;-Wall;-Wextra")
  add_compile_options("-Wno-redundant-move;-Wno-sign-compare")

  if (WITH_DEBUG_INFO)
    add_compile_options("-ggdb;-fno-omit-frame-pointer")
  endif()
endif()

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
include(KDEInstallDirs)

find_package(Qt5 COMPONENTS Core DBus Quick Charts Widgets Network Svg REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(KF5Auth REQUIRED)
find_package(KF5Archive REQUIRED)
find_package(Botan REQUIRED)

add_subdirectory(src)

# Tests
include(CTest)
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

# resources
string(TOLOWER "${PROJECT_NAME}" ICON_NAME)
install(FILES resources/icon/app_16.svg DESTINATION share/icons/hicolor/16x16/apps RENAME ${ICON_NAME}.svg)
install(FILES resources/icon/app_22.svg DESTINATION share/icons/hicolor/22x22/apps RENAME ${ICON_NAME}.svg)
install(FILES resources/icon/app_24.svg DESTINATION share/icons/hicolor/24x24/apps RENAME ${ICON_NAME}.svg)
install(FILES resources/icon/app_32.svg DESTINATION share/icons/hicolor/32x32/apps RENAME ${ICON_NAME}.svg)
install(FILES resources/icon/app_48.svg DESTINATION share/icons/hicolor/48x48/apps RENAME ${ICON_NAME}.svg)
install(FILES resources/icon/app_64.svg DESTINATION share/icons/hicolor/64x64/apps RENAME ${ICON_NAME}.svg)
install(FILES resources/icon/app_72.svg DESTINATION share/icons/hicolor/72x72/apps RENAME ${ICON_NAME}.svg)
install(FILES resources/icon/app_96.svg DESTINATION share/icons/hicolor/96x96/apps RENAME ${ICON_NAME}.svg)
install(FILES resources/icon/app_128.svg DESTINATION share/icons/hicolor/128x128/apps RENAME ${ICON_NAME}.svg)
install(FILES resources/icon/app_192.svg DESTINATION share/icons/hicolor/192x192/apps RENAME ${ICON_NAME}.svg)
install(FILES resources/icon/app_256.svg DESTINATION share/icons/hicolor/256x256/apps RENAME ${ICON_NAME}.svg)
install(FILES resources/launcher.desktop DESTINATION share/applications RENAME org.corectrl.corectrl.desktop)
install(FILES resources/org.corectrl.corectrl.appdata.xml DESTINATION share/metainfo)
