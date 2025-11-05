cmake_minimum_required(VERSION 3.10)
project(bej_parser C)

set(CMAKE_C_STANDARD 99)

# dirs
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
set(JSON_DIR ${CMAKE_SOURCE_DIR}/json)

include_directories(${INCLUDE_DIR})

# src files
set(SOURCES
    ${SRC_DIR}/main.c
    ${SRC_DIR}/bej_parse.c
    ${SRC_DIR}/dictionary.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

# make bin and json dirs
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${JSON_DIR}
)

# warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

# debug stuff
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_MODE=1)
endif()
