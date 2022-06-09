cmake_minimum_required(VERSION 3.22)
project(mas)

set(CMAKE_CXX_STANDARD 17)

add_executable(mas
        src/main.cpp
        src/diagn.cpp
        src/cursor.cpp
        src/pass1.cpp
        src/mic_instr_bits.cpp
        src/pass2.cpp)