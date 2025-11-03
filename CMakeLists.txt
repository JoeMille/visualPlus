cmake_minimum_required(VERSION 3.16)
project(VisualPlusServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

include(FetchContent)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY http://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.14.1
)
FetchContent_MakeAvailable(httplib)

add_executable(server src/main.cpp)

target_link_libraries(server
    httplib::httplib
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread        
)

add_custom_command(TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/public ${CMAKE_BINARY_DIR}/public
)