# Brainstorming

This file is used to document your thoughts, approaches and research conducted across all tasks in the Technical Assessment.

## Firmware

Task 1:
Followed 
https://github.com/xR3b0rn/dbcppp
to figre out how to add the submodule locally, kept track of the other required installs
Added those and dbcppp to the dockerfile.

Struggled with paths like I always do to open log and to load the different bus; ended up using realtive path and it works locally (error handling)

Decied on structure
Read line -> extra timestapm, canid, check for dbc(s !) with matching canid; then go through each message to find matching then process all signals

Helper function adapted from another project mine for hex string to byte
Chatgpt helped with dealing with the string from log ( pretty proud of my std use)
Chatgpt helped with the tedious formating to outpul file (Full 6 for timestamp but no trailing for data)


Realised that for a working build we should not copy everything, only copied main dumplog and DBC files.
ALso orgnanised it to work with a build folder and the relative paths from earlier.


Task 2:
Actually could not find a chip that fit everything; potentially missunderstanding the flash mem requirement ?

Task 3:
Broke apart parseCanline and hextobyte to test them.

To test 
Proper extraction of sensor values
Handling of multiple DBC files defining the same CAN ID

It took a while to figure out how to test and link the dbcppp library to test parser in the test file

I mainly test my helper functions then tested my parser with the known input and outputs
Also strugled when I broke apart parser and main, cpp files don't like including other cpp file, that's why we have parser.hpp
How to run:
cmake ..
maje -j
./test_parser
## Spyder

## Cloud