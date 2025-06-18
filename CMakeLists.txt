cmake_minimum_required(VERSION 3.10)
project(crypto_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH})

add_library(ElGamal SHARED src/gaml/gaml.cpp)
add_library(HillCipher SHARED src/hill/hill.cpp)
add_library(ShamirProtocol SHARED src/shamir/shamir.cpp)

add_executable(CryptoMain src/main.cpp)
target_link_libraries(CryptoMain ElGamal HillCipher ShamirProtocol)

install(TARGETS CryptoMain DESTINATION bin)
install(TARGETS ElGamal HillCipher ShamirProtocol DESTINATION lib)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "YourName")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.31), libstdc++6, libssl3")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
include(CPack)
