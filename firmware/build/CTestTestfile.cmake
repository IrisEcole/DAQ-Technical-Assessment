# CMake generated Testfile for 
# Source directory: /home/risd/DAQ-Technical-Assessment/firmware
# Build directory: /home/risd/DAQ-Technical-Assessment/firmware/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[parser_test]=] "/home/risd/DAQ-Technical-Assessment/firmware/build/test_parser")
set_tests_properties([=[parser_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/risd/DAQ-Technical-Assessment/firmware/CMakeLists.txt;39;add_test;/home/risd/DAQ-Technical-Assessment/firmware/CMakeLists.txt;0;")
subdirs("solution")
subdirs("_deps/catch2-build")
