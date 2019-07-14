cmake_minimum_required(VERSION 3.3)

project(CoreCtrl LANGUAGES CXX)
set(PROJECT_VERSION 1.0.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

list(APPEND TARGET_COMMON_COMPILE_FEATURES
  cxx_std_17              # C++17
)
add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)
add_compile_options("$<$<CONFIG:Debug>:-W;-Wall;-Wextra;-Wpedantic>")

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
include(KDEInstallDirs)

find_package(Qt5 COMPONENTS Core DBus Quick Charts Widgets Network REQUIRED)
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
install(FILES resources/launcher.desktop DESTINATION share/applications RENAME ${PROJECT_NAME}.desktop)
install(FILES resources/icon/app_16.svg DESTINATION share/icons/hicolor/16x16/apps RENAME ${PROJECT_NAME}.svg)
install(FILES resources/icon/app_22.svg DESTINATION share/icons/hicolor/22x22/apps RENAME ${PROJECT_NAME}.svg)
install(FILES resources/icon/app_24.svg DESTINATION share/icons/hicolor/24x24/apps RENAME ${PROJECT_NAME}.svg)
install(FILES resources/icon/app_32.svg DESTINATION share/icons/hicolor/32x32/apps RENAME ${PROJECT_NAME}.svg)
install(FILES resources/icon/app_48.svg DESTINATION share/icons/hicolor/48x48/apps RENAME ${PROJECT_NAME}.svg)
install(FILES resources/icon/app_64.svg DESTINATION share/icons/hicolor/64x64/apps RENAME ${PROJECT_NAME}.svg)
install(FILES resources/icon/app_72.svg DESTINATION share/icons/hicolor/72x72/apps RENAME ${PROJECT_NAME}.svg)
install(FILES resources/icon/app_96.svg DESTINATION share/icons/hicolor/96x96/apps RENAME ${PROJECT_NAME}.svg)
install(FILES resources/icon/app_128.svg DESTINATION share/icons/hicolor/128x128/apps RENAME ${PROJECT_NAME}.svg)
install(FILES resources/icon/app_196.svg DESTINATION share/icons/hicolor/196x196/apps RENAME ${PROJECT_NAME}.svg)
install(FILES resources/icon/app_256.svg DESTINATION share/icons/hicolor/256x256/apps RENAME ${PROJECT_NAME}.svg)
