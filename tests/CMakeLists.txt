option(BUILD_SPLIT_TESTS "Build one test executable per test file" OFF)

# Compile definitions
list(APPEND TESTS_COMPILE_DEFINITIONS
  # https://github.com/catchorg/Catch2/blob/master/docs/configuration.md
  CATCH_CONFIG_FAST_COMPILE

  # https://github.com/zuhd-org/easyloggingpp/blob/master/README.md
  ELPP_DISABLE_LOGS
)

# inlude directories for all tests
list(APPEND TESTS_INCLUDE_DIRECTORIES
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/catch>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/trompeloeil>"
  "${PROJECT_SOURCE_DIR}/src/3rdparty"
  "${PROJECT_SOURCE_DIR}/src"
)

# catch main
add_library(catch_main OBJECT "src/main.cpp")
target_include_directories(catch_main PRIVATE ${TESTS_INCLUDE_DIRECTORIES})
target_compile_features(catch_main PRIVATE ${TARGET_COMMON_COMPILE_FEATURES})

# test_* files
file(GLOB test_files "src/test_*.cpp")

# generate test_all executable
add_executable(test_all $<TARGET_OBJECTS:catch_main> ${test_files})
target_include_directories(test_all PRIVATE ${TESTS_INCLUDE_DIRECTORIES})
target_compile_features(test_all PRIVATE ${TARGET_COMMON_COMPILE_FEATURES})
target_compile_definitions(test_all PRIVATE ${TESTS_COMPILE_DEFINITIONS})
target_link_libraries(test_all PRIVATE Qt5::Core corectrl_lib)

add_test(NAME "test_all"
         COMMAND test_all
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# generate one executable per test_* file
if(BUILD_SPLIT_TESTS)
  foreach(file ${test_files})
    get_filename_component(file_name ${file} NAME_WE)
    string(REGEX REPLACE "test_([^$]+)" "test_\\1" test_name ${file_name})

    add_executable(${test_name} $<TARGET_OBJECTS:catch_main> ${file})
    target_include_directories(${test_name} PRIVATE ${TESTS_INCLUDE_DIRECTORIES})
    target_compile_features(${test_name} PRIVATE ${TARGET_COMMON_COMPILE_FEATURES})
    target_compile_definitions(${test_name} PRIVATE ${TESTS_COMPILE_DEFINITIONS})
    target_link_libraries(test_all PRIVATE Qt5::Core corectrl_lib)

    add_test(NAME "${test_name}"
             COMMAND ${test_name}
             WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endforeach()
endif()
