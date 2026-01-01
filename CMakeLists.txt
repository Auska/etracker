cmake_minimum_required(VERSION 3.10)
project(etracker C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -lm")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -march=native")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SERVER_SOURCES
    src/server.c
    src/sem.c
    src/alloc.c
    src/uri.c
    src/socket.c
    src/string.c
    src/thread_client_tcp.c
    src/time.c
    src/block.c
    src/stats.c
    src/socket_tcp.c
    src/socket_udp.c
    src/thread_garbage.c
    src/data_structure.c
    src/data_garbage.c
    src/thread_client_udp.c
    src/socket_udp_structure.c
    src/equeue.c
    src/socket_garbage.c
    src/interval.c
    src/udp_request.c
    src/rps.c
    src/list.c
    src/data.c
    src/basic.c
    src/base64.c
    src/argument.c
    src/thread.c
    src/math.c
    src/exit_code.c
    src/sha1.c
    src/websocket.c
    src/geoip.c
)

set(CLIENT_SOURCES
    src/client.c
)

set(TEST_SOURCES
    tests/test.c
    src/alloc.c
    src/block.c
    src/list.c
    src/sem.c
    src/base64.c
    src/sha1.c
    src/string.c
    src/exit_code.c
    src/geoip.c
)

# Get git revision for versioning
find_package(Git)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_REVISION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    add_definitions(-DREVISION="${GIT_REVISION}")
else()
    add_definitions(-DREVISION="UNKNOWN")
endif()

# Build server executable
add_executable(etracker ${SERVER_SOURCES})
target_link_libraries(etracker pthread m)

# Build client executable
add_executable(client ${CLIENT_SOURCES})

# Build test executable
add_executable(test ${TEST_SOURCES})
target_link_libraries(test pthread m)

# Installation
install(TARGETS etracker client test
    RUNTIME DESTINATION bin
)

# Install web files
install(DIRECTORY web/
    DESTINATION share/etracker/web
)

# Install scripts
install(PROGRAMS
    scripts/run
    scripts/run_chroot
    scripts/initWeb
    scripts/aauto_tests_run
    scripts/ffunctional_tests_run
    DESTINATION bin
)