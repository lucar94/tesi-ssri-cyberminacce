cmake_minimum_required(VERSION 3.10)
project(decryptor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

add_executable(decryptor src/main.cpp)
target_link_libraries(decryptor PRIVATE OpenSSL::SSL OpenSSL::Crypto)