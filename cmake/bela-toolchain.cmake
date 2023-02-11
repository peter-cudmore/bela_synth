message("using xc-bela-toolchain")
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(triple arm-linux-gnueabihf)
set(CC  /usr/local/linaro/arm-bela-linux-gnueabihf/bin/arm-linux-gnueabihf-)

set(CMAKE_C_COMPILER            ${CC}gcc)
set(CMAKE_CXX_COMPILER          ${CC}g++)
set(CMAKE_AR                    ${CC}ar)
set(CMAKE_ASM_COMPILER          ${CC}as)
set(CMAKE_LINKER                ${CC}ld)
set(CMAKE_OBJCOPY               ${CC}objcopy)
set(CMAKE_RANLIB                ${CC}ranlib)
set(CMAKE_SIZE                  ${CC}size)
set(CMAKE_STRIP                 ${CC}strip)
set(CMAKE_TRY_COMPILE_TARGET_TYPE  STATIC_LIBRARY)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER_TARGET ${triple})

set(XC_ROOT /usr/local/linaro/BelaSysroot)

set(CMAKE_CROSSCOMPILING                    ON)
set(CMAKE_FIND_ROOT_PATH                  ${XC_ROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM     NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY     BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE     BOTH)


set(XC_INCLUDE /usr/local/linaro/BelaSysroot/usr/lib/gcc/arm-linux-gnueabihf/6.3.0/include/)
set(XC_SYSROOT ${XC_ROOT})
set(BELA_ROOT ../Bela/)


include_directories(
        BEFORE SYSTEM ${XC_INCLUDE}
        ${XC_ROOT}/usr/include
        ${XC_SYSROOT}/root/Bela/include
        ${XC_SYSROOT}/root/Bela
        ${BELA_ROOT}

)

set(BELA_LINK_FLAGS "${BELA_LINK_FLAGS} -L${XC_SYSROOT}/root/Bela/lib -L${XC_SYSROOT}/usr/local/lib -Wl,--no-as-needed -L${XC_SYSROOT}/usr/xenomai/lib" )
set(BELA_LINK_FLAGS "${BELA_LINK_FLAGS} -lprussdrv -lasound -lseasocks -lNE10 -lmathneon")
set(BELA_LINK_FLAGS "${BELA_LINK_FLAGS} -l:libbela.a -l:libbelaextra.a")
set(BELA_LINK_FLAGS "${BELA_LINK_FLAGS} -L${XC_SYSROOT}/usr/lib/gcc/arm-linux-gnueabihf/6.3.0  -B${XC_SYSROOT}/usr/lib/gcc/arm-linux-gnueabihf/6.3.0  -latomic")
set(BELA_LINK_FLAGS "${BELA_LINK_FLAGS} -Wl,-rpath-link,${XC_SYSROOT}/lib/arm-linux-gnueabihf")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${BELA_LINK_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${BELA_LINK_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${BELA_LINK_FLAGS}")


# possibily useful?
# set(CMAKE_STAGING_PREFIX /home/devel/stage)
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)



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

