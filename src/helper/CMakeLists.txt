find_package(PkgConfig REQUIRED)
find_package(Qt5 5.15 COMPONENTS Core DBus REQUIRED)
find_package(Botan REQUIRED)

set(DBUS_DATADIR_PREFIX_DIR ${CMAKE_INSTALL_FULL_DATADIR})
option(INSTALL_DBUS_FILES_IN_PREFIX "Use installation prefix for D-Bus files" OFF)

# Find dbus
if(NOT INSTALL_DBUS_FILES_IN_PREFIX)
  pkg_check_modules(DBUS REQUIRED dbus-1)
  execute_process(
    COMMAND pkg-config --variable=datadir dbus-1
    RESULT_VARIABLE DBUS_DATADIR_PREFIX_DIR_RESULT
    OUTPUT_VARIABLE DBUS_DATADIR_PREFIX_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(NOT DBUS_DATADIR_PREFIX_DIR_RESULT EQUAL "0")
    message(FATAL_ERROR "Failed to retrieve D-Bus `datadir` variable using pkg-config")
  endif()
endif()
message("D-Bus files will be installed into ${DBUS_DATADIR_PREFIX_DIR}/dbus-1")

# Find polkit
pkg_check_modules(POLKIT REQUIRED polkit-gobject-1)
execute_process(
    COMMAND pkg-config --variable=policydir polkit-gobject-1
    RESULT_VARIABLE POLKIT_POLICY_INSTALL_DIR_RESULT
    OUTPUT_VARIABLE POLKIT_POLICY_INSTALL_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT POLKIT_POLICY_INSTALL_DIR_RESULT EQUAL "0")
  message(FATAL_ERROR "Failed to retrieve Polkit `policydir` variable using pkg-config")
endif()

list(APPEND PROCESS_MONITOR_SRC
  pmon/processmonitor.cpp
  pmon/procpidsolver.cpp
  pmon/nlprocexecsocket.cpp
  pmon/nlprocexecmonitor.cpp
  pmon/processregistry.cpp
  pmon/appregistry.cpp
  pmon/msgdispatcher.cpp
)

list(APPEND SYSTEM_CONTROL_SRC
  sysctl/sysfswriter.cpp
  sysctl/msgreceiver.cpp
)

set(CMAKE_AUTOMOC ON)
add_executable(corectrl_helperkiller
  helperkiller.cpp
  polkit.cpp
  ${3RDPARTY_COMMON_SRC}
)
set(CMAKE_AUTOMOC OFF)

target_include_directories(corectrl_helperkiller PRIVATE
  ${POLKIT_INCLUDE_DIRS}
  ${3RDPARTY_INCLUDE_DIRECTORIES}
)
target_compile_features(corectrl_helperkiller PRIVATE ${TARGET_COMMON_COMPILE_FEATURES})
target_compile_definitions(corectrl_helperkiller PRIVATE ${3RDPARTY_COMMON_DEFINITIONS})
target_link_libraries(corectrl_helperkiller PRIVATE
  Qt5::Core
  Qt5::DBus
  ${POLKIT_LIBRARIES}
)

configure_file(org.corectrl.helperkiller.service.in org.corectrl.helperkiller.service)
install(TARGETS corectrl_helperkiller DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/corectrl")
install(FILES org.corectrl.helperkiller.conf DESTINATION "${DBUS_DATADIR_PREFIX_DIR}/dbus-1/system.d")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/org.corectrl.helperkiller.service" DESTINATION "${DBUS_DATADIR_PREFIX_DIR}/dbus-1/system-services")
install(FILES org.corectrl.helperkiller.policy DESTINATION "${POLKIT_POLICY_INSTALL_DIR}")

set(CMAKE_AUTOMOC ON)
add_executable(corectrl_helper
  helper.cpp
  polkit.cpp
  ${3RDPARTY_COMMON_SRC}
  ${COMMON_SRC}
  ${PROCESS_MONITOR_SRC}
  ${SYSTEM_CONTROL_SRC}
)
set(CMAKE_AUTOMOC OFF)

target_include_directories(corectrl_helper PRIVATE
  ${Botan_INCLUDE_DIRS}
  ${POLKIT_INCLUDE_DIRS}
  ${3RDPARTY_INCLUDE_DIRECTORIES}
  ${PROJECT_SOURCE_DIR}/src
)
target_compile_features(corectrl_helper PRIVATE ${TARGET_COMMON_COMPILE_FEATURES})
target_compile_definitions(corectrl_helper PRIVATE ${3RDPARTY_COMMON_DEFINITIONS})
target_link_libraries(corectrl_helper PRIVATE
  Qt5::Core
  Qt5::DBus
  stdc++fs
  pthread
  ${Botan_LIBRARIES}
  ${POLKIT_LIBRARIES}
)

configure_file(org.corectrl.helper.service.in org.corectrl.helper.service)
install(TARGETS corectrl_helper DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/corectrl")
install(FILES org.corectrl.helper.conf DESTINATION "${DBUS_DATADIR_PREFIX_DIR}/dbus-1/system.d")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/org.corectrl.helper.service" DESTINATION "${DBUS_DATADIR_PREFIX_DIR}/dbus-1/system-services")
install(FILES org.corectrl.helper.policy DESTINATION "${POLKIT_POLICY_INSTALL_DIR}")
