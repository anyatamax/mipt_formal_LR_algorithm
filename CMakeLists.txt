cmake_minimum_required(VERSION 3.16)
project(mipt_formal_LR_algorithm)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" ${CMAKE_SOURCE_DIR}/cmake)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")

include(CodeCoverage)

find_package(GTest REQUIRED)
find_package(GMock REQUIRED)
find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${GMOCK_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

add_executable(mipt_formal_LR_algorithm main.cpp)
target_link_libraries(mipt_formal_LR_algorithm Library)

target_include_directories(mipt_formal_LR_algorithm PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(${PROJECT_SOURCE_DIR}/Test)

#setup_target_for_coverage(tests ${PROJECT_SOURCE_DIR}/bin/Test coverage)
#
#
#enable_testing()
#add_test(Test0 ${PROJECT_SOURCE_DIR}/bin/Test)