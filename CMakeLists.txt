cmake_minimum_required(VERSION 3.7)
project(bass_synth)

file(GLOB SOURCES
        src/kernel/*
)

add_executable(bass_synth
        ${SOURCES}
        src/main.cpp
        src/server.cpp
        src/patch.cpp
        src/midi_io.cpp
        src/message_dispatch.cpp
)

target_include_directories(bass_synth
        PUBLIC
        include
        external/include
)


set_target_properties(bass_synth
    PROPERTIES POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(bass_synth
        bela
       belaextra
       rt
        zmq
)


