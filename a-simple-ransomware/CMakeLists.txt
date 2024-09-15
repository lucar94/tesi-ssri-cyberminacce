cmake_minimum_required(VERSION 3.10)
project(a_simple_ransomware)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

add_executable(a_simple_ransomware src/main.cpp src/C2CCommunication.cpp)
target_link_libraries(a_simple_ransomware PRIVATE OpenSSL::SSL OpenSSL::Crypto CURL::libcurl)