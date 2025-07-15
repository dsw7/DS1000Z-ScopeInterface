cmake_minimum_required(VERSION 3.18)
project(DS1000Z-ScopeInterface VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(PROJECT_VERSION="${PROJECT_VERSION}")
add_compile_options(-Wall -Wextra -pedantic -Werror)

message(STATUS "Checking for toml++ header file")
set(TOMLPLUSPLUS_HPP_DL "${CMAKE_SOURCE_DIR}/external/toml.hpp")

if(EXISTS ${TOMLPLUSPLUS_HPP_DL})
  message(STATUS "${TOMLPLUSPLUS_HPP_DL} already exists. Skipping download")
else()
  message(STATUS "Downloading toml.hpp")
  file(
    DOWNLOAD
    "https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp"
    ${TOMLPLUSPLUS_HPP_DL}
    SHOW_PROGRESS
  )
endif()

include_directories(src external)
add_executable(
  scope
  main.cpp
  parameters.cpp
  scope.cpp
  tcp_conn.cpp
  utils.cpp
  workflows.cpp
)

find_package(fmt REQUIRED)
target_link_libraries(scope fmt::fmt) 

set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local")
install(TARGETS scope DESTINATION bin)
