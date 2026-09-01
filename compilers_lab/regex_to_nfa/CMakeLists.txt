cmake_minimum_required(VERSION 3.16)
project(regex_to_nfa CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(regex_to_nfa
    ./src/main.cpp
    ./src/nfa.cpp
    ./src/regex.cpp
)
