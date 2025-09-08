cmake_minimum_required(VERSION 3.22)
project(firmware)



add_subdirectory(solution)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/solution
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove -G "${CMAKE_GENERATOR}" cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove -G "${CMAKE_GENERATOR}" Makefile
    COMMAND ${CMAKE_COMMAND} -E remove -G "${CMAKE_GENERATOR}" DartConfiguration.tcl
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.4.0  # <-- use an actual tag
)

FetchContent_MakeAvailable(catch2)

# Tests
add_executable(test_parser
    tests/test_parser.cpp
    ../solution/parser.cpp   # add this line
)
target_link_libraries(test_parser PRIVATE Catch2::Catch2WithMain)
# Include dbcppp headers for test
target_include_directories(test_parser PRIVATE /usr/local/include/dbcppp)
target_link_libraries(test_parser PRIVATE /usr/local/lib/libdbcppp.so Catch2::Catch2)

enable_testing()
add_test(NAME parser_test COMMAND test_parser)