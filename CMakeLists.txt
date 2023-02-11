cmake_minimum_required(VERSION 3.22)
project(bass_synth)

set(CMAKE_CXX_STANDARD 17)
include_directories(external/include)

add_executable(bass_synth main.cpp)
