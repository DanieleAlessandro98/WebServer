# WebServer

## Overview

WebServer is a simple HTTP server written in C, socket-based, capable of handling multiple concurrent connections and simple HTTP requests and responses, such as requests for static files. It uses a watcher called `fdwatch` to monitor read and write events on file descriptors. It is also designed to be compatible with Windows and POSIX (such as Linux) operating systems, thanks to compatibility macros defined in the `definitions.h` file.

## Key features

- **Simultaneous connection handling**: The server can handle multiple connections simultaneously using `fdwatch`.
- **GET method support**: Can serve static files from the file system and return appropriate HTTP responses such as `200 OK`, `404 Not Found` or `400 Bad Request`.
- **Non-blocking sockets**: Uses non-blocking sockets.
- **Error handling**: Provides error messages for failed socket operations or malformed HTTP requests.
- **Dynamic Buffers**: Uses dynamic buffers to adapt to the size of incoming and outgoing data, optimising memory usage.
- **Cross-Platform**: Compatible with both Windows and POSIX operating systems.


## Project structure

The project is structured and organised as follows:

```
WebServer/
│
├── bin/             # Executable directory
├── build/           # CMake generated compilation files
├── public/          # Static files (HTML, CSS, etc.)
├── src/             # Source code
└── CMakeLists.txt   # CMake Configuration
```

## Main files

- **`main.c`**: Contains the main loop of the server, handles listening for connections and shutting down the server.

- **`socket.c`**: Handles socket creation and configuration. This includes socket creation, setting options such as `SO_REUSEADDR` and `NONBLOCK`, binding and listening for new connections, and handling incoming connections.

- **`fdwatch.c`**: Implements a wrapper around `select()` function, used to monitor read and write events on sockets. This approach allows the server to efficiently handle multiple concurrent connections without having to create a separate thread for each connection.

- **`connection.c`**: Handles connections with clients. The main functions are `process_new_connection()` for new connections, `process_client_read()` for reading data sent by the client and `process_client_write()` for writing data.

- **`http.c`**: Implements the logic for processing HTTP requests and sending the appropriate responses. It supports the `GET` method for serving static files and handles common HTTP errors such as `400 Bad Request` and `404 Not Found`.

- **`network_io.c`**: Handles sending and receiving data on sockets.

- **`buffer.c`**: Manages the dynamic reallocation of buffers for sending and receiving, adapting to variable data sizes and optimising memory usage.

- **`definitions.h`**: Defines constants and macros to ensure portability and consistency of code between different platforms (Windows and Unix-like). It also specifies limits and maximum sizes for the HTTP protocol.



## Requirements

- **CMake** 3.10 or later
- **C compiler** (MinGW, MSVC, etc.)

## Installation and Configuration

To install and compile WebServer, follow the steps below:

1. **Clone the repository:**
   ```bash
   git clone https://github.com/DanieleAlessandro98/WebServer.git
   cd WebServer
   ```

2. **Compile the project:**
   ```bash
   cd build
   cmake .. && cmake --build .
   ```

3. **Run the application:**
   The executable will be in the `bin` folder.

## How to use

Once the server is up and running, it can be accessed via a browser. By default, the server responds with HTML files in the `public/` folder.

Example:
```
http://localhost:8080
```
