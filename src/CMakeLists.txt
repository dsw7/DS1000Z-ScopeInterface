cmake_minimum_required(VERSION 3.18)
project(DS1000Z-ScopeInterface VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

string(TIMESTAMP BUILD_DATE)
message(STATUS "Will add the following timestamp to binary: ${BUILD_DATE}")

add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")
add_compile_definitions(PROJECT_VERSION="${PROJECT_VERSION}")
add_compile_definitions(BUILD_DATE="${BUILD_DATE}")

add_compile_options(-Wall -Wextra -pedantic -Werror)

include_directories(src)
add_executable(
  scope
  client.cpp
  main.cpp
  parameters.cpp
  workflows.cpp
)

set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local")
install(TARGETS scope DESTINATION bin)
