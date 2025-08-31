# Config
cmake_minimum_required( VERSION 3.21 )
add_compile_options(-std=c++20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Project
project(Koodinmurtaja)

add_executable(Koodinmurtaja 
src/main.cpp
src/room.cpp
)
# Dependencies

#find_package(OpenSSL REQUIRED)
# nginx handles SSL

# find_package(ZLIB REQUIRED) # Not required since this almost purely uses websockets anyways

find_package(nlohmann_json CONFIG REQUIRED)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
CPMAddPackage(Crow
  VERSION 1.2.1
  GITHUB_REPOSITORY CrowCpp/Crow
  OPTIONS
  "CROW_BUILD_EXAMPLES Off"
  "CROW_BUILD_TOOLS Off"
  "CROW_BUILD_TESTS Off"
  "CROW_BUILD_DOCS Off"
  "CROW_ENABLE_COMPRESSION Off"
  "CROW_ENABLE_SSL Off"
)

target_link_libraries( Koodinmurtaja
  PRIVATE
  #OpenSSL::SSL
  #OpenSSL::Crypto
  #ZLIB::ZLIB
  Crow::Crow
  nlohmann_json::nlohmann_json
)
