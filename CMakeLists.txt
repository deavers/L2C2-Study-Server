cmake_minimum_required(VERSION 3.10)
project(L2C2StudyServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Files
add_executable(l2c2_login
    src/main.cpp
    src/net/tcp_server.cpp
    src/login/login_server.cpp
)
# pthread for threads
find_package(Threads REQUIRED)
target_link_libraries(l2c2_login PRIVATE Threads::Threads)
