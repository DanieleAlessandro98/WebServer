cmake_minimum_required(VERSION 3.10)

# Project name
project(WebServer)

# Output directory binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Create the executable
add_executable(${PROJECT_NAME}
        src/main.c
        src/winsock_utils.c
        src/socket.c
)

target_link_libraries(${PROJECT_NAME} Ws2_32)
