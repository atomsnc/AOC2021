cmake_minimum_required(VERSION 3.21)

#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(BUILDABLE AOCDay-5)
add_executable(${BUILDABLE} main.cpp)
add_custom_command(
        TARGET ${BUILDABLE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/test.txt
        ${CMAKE_CURRENT_BINARY_DIR}/test.txt)
add_custom_command(
        TARGET ${BUILDABLE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/input.txt
        ${CMAKE_CURRENT_BINARY_DIR}/input.txt)
