# Minimum version required for modern C++ features
cmake_minimum_required(VERSION 3.10)

# Project definition
project(LADOHD_ETW_Agent VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define the executable and its source files
add_executable(etw_agent 
    main.cpp 
    TcpClient.cpp 
    EtwParser.cpp
)

# Link essential Windows Kernel and Networking libraries
target_link_libraries(etw_agent 
    ws2_32.lib 
    tdh.lib
)