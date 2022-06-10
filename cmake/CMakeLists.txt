cmake_minimum_required(VERSION 3.22)
project(k8_18)

set(CMAKE_CXX_STANDARD 17)

add_executable(emu
        ../emu/src/main.cpp
        ../emu/src/cpu.cpp
        ../emu/src/bus.cpp
        ../emu/src/log.cpp
        ../emu/src/devs/ram.cpp)

add_executable(mas
        ../mas/src/main.cpp
        ../mas/src/diagn.cpp
        ../mas/src/cursor.cpp
        ../mas/src/pass1.cpp
        ../mas/src/mic_instr_bits.cpp
        ../mas/src/pass2.cpp)