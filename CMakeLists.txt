cmake_minimum_required(VERSION 3.28)

project(test_gps_teseo C CXX ASM)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts -fcommon")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-allow-multiple-definition")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(callbackmanager
  GIT_REPOSITORY "https://github.com/jancumps/callbackmanager.git"
  GIT_TAG "origin/main"   # it's much better to use a specific Git revision or Git tag for reproducibility
)
FetchContent_MakeAvailable(callbackmanager)

enable_testing()

add_executable(${CMAKE_PROJECT_NAME}_test)
target_sources(${CMAKE_PROJECT_NAME}_test
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/teseo/teseo.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/test_gps_teseo_lib.cpp
        ${callbackmanager_SOURCE_DIR}/test/callbackmanager_test.cpp
)
target_sources(${CMAKE_PROJECT_NAME}_test
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/teseo/teseo_iface.cpp
        ${callbackmanager_SOURCE_DIR}/callbackmanager.cpp
)

target_link_libraries( ${CMAKE_PROJECT_NAME}_test
        GTest::gtest_main
        GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(${CMAKE_PROJECT_NAME}_test)
