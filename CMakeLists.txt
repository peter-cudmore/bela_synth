cmake_minimum_required(VERSION 3.7)
project(bass_synth)

set(CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES src/*.cpp)

add_executable(bass_synth ${SOURCES})
target_include_directories(bass_synth
        PUBLIC
        include
        external/include)


target_link_libraries(bass_synth
        bela
        belaextra
        rt
)


