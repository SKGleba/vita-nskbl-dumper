cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(nskbl_dumper)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -Wno-unused-variable -Wno-implicit-function-declaration -O3 -nostdlib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

add_executable(nskbl_dumper
  kernel.c
)

target_link_libraries(nskbl_dumper
  SceIofilemgrForDriver_stub
)

vita_create_self(nskbl_dump.skprx nskbl_dumper CONFIG exports.yml UNSAFE)
