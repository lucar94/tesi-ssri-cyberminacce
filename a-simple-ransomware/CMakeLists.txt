cmake_minimum_required(VERSION 3.10)
project(MyCppProject)

# Imposta il C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Trova la libreria OpenSSL
find_package(OpenSSL REQUIRED)

# Aggiungi eseguibile
add_executable(a_simple_ransomware src/main.cpp)

# Linka le librerie OpenSSL
target_link_libraries(a_simple_ransomware OpenSSL::SSL OpenSSL::Crypto)