cmake_minimum_required (VERSION 2.8)

project (chk_update_filename_valid)

SET(CMAKE_SOURCE_DIR .)
SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")
add_executable(chk_update_filename_valid chk_update_filename_valid.c)


