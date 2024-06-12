cmake_minimum_required(VERSION 3.12)
project(
  "CSV"
  DESCRIPTION "A simple library for writing csv files in C"
  VERSION 0.0.1)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "$ENV{HOME}/.local")
include_directories("${CMAKE_PREFIX_PATH}/include")

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_C_STANDARD 99)

add_library(csv csv.c)
add_executable(main main.c)
target_link_libraries(main csv)

find_path(CUNIT_INCLUDE_DIR NAMES CUnit/CUnit.h)
mark_as_advanced(CUNIT_INCLUDE_DIR)

find_library(CUNIT_LIBRARY NAMES cunit libcunit cunitlib)
mark_as_advanced(CUNIT_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CUnit DEFAULT_MSG CUNIT_LIBRARY
                                  CUNIT_INCLUDE_DIR)

if(CUNIT_FOUND)
  enable_testing()
  add_executable(test_csv test.c)

  target_link_libraries(test_csv -L${CMAKE_BINARY_DIR}/lib ${CUNIT_LIBRARY} csv)
  set_target_properties(test_csv PROPERTIES LINK_FLAGS -Wl,-L${CUNIT_LIBRARY},-rpath=${CMAKE_BINARY_DIR}/lib)

  add_test(test_csv ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif(CUNIT_FOUND)
