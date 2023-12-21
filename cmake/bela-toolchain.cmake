message("using xc-bela-toolchain")
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(triple arm-linux-gnueabihf)

set(CMAKE_CXX_STANDARD  14)
set(CMAKE_C_COMPILER            /usr/bin/clang-16)
set(CMAKE_CXX_COMPILER          /usr/bin/clang++-16)

set(CMAKE_TRY_COMPILE_TARGET_TYPE  STATIC_LIBRARY)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER_TARGET ${triple})
set(DEFAULT_SYSROOT /mnt/bela)
# set(CMAKE_AR llvm-ar)

set(BELA_ROOT ${DEFAULT_SYSROOT}/root/Bela)
set(GCC_TOOLCHAIN  /usr/local/linaro/arm-bela-linux-gnueabihf)
set(GCC_TOOLCHAIN_VERSION  6.3.1)
set(CMAKE_CROSSCOMPILING                  TRUE)

set(CMAKE_FIND_ROOT_PATH                  ${DEFAULT_SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM     NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY     BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE     BOTH)
set(CMAKE_SYSROOT                           ${DEFAULT_SYSROOT})
set(CMAKE_C_COMPILER_EXTERNAL_TOOLCHAIN     ${GCC_TOOLCHAIN})
set(CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN   ${GCC_TOOLCHAIN})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


set(CMAKE_BUILD_RPATH
        ${DEFAULT_SYSROOT}/lib
        ${DEFAULT_SYSROOT}/lib/${triple}
        ${DEFAULT_SYSROOT}/usr/lib/${triple}
        ${DEFAULT_SYSROOT}/usr/xenomai/lib
        ${DEFAULT_SYSROOT}/usr/lib
)


set(CMAKE_CXX_FLAGS "-stdlib=libstdc++ --gcc-toolchain=${GCC_TOOLCHAIN} -march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon -ftree-vectorize -ffast-math -fno-exceptions")

set(CMAKE_CXX_LINK_FLAGS "-fuse-ld=${GCC_TOOLCHAIN}/bin/${triple}-ld -stdlib=libstdc++ --gcc-toolchain=${GCC_TOOLCHAIN}")

link_directories(
    ${DEFAULT_SYSROOT}/usr/xenomai/lib
    ${BELA_ROOT}/lib
)

link_libraries(
        cobalt
        modechk
        pthread
        rt
        mathneon
)



#bela-config for details
#note: most of these are in the main cmake file as conditions on -DBELA=on

#/root/Bela/resources/bin/bela-config --defines
#-DXENOMAI_SKIN_posix
#/root/Bela/resources/bin/bela-config --includes
#/root/Bela/include
#/root/Bela/resources/bin/bela-config --libraries
#-L/usr/xenomai/lib -L/root/Bela/lib -lbela -lcobalt -lmodechk -lpthread -lrt
#/root/Bela/resources/bin/bela-config --cflags
#-march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon -ftree-vectorize -ffast-math
#/root/Bela/resources/bin/bela-config --cxxflags
#-march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon -ftree-vectorize -ffast-math

