cmake_minimum_required(VERSION 3.10)

# Project name
project(WebServer)

# Output directory binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

# Source files
set(SOURCES
        src/main.c
        src/socket.c
        src/address.c
        src/network_io.c
        src/fdwatch.c
        src/connection.c
        src/http.c
        src/file.c
        src/mime.c
        src/buffer.c
)

# Add specific OS source files
if (WIN32)
    list(APPEND SOURCES
                src/winsock_utils.c
        )
endif()

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add specific OS libs
if (WIN32)
    target_link_libraries(${PROJECT_NAME} Ws2_32)
endif()
