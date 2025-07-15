cmake_minimum_required(VERSION 3.18)
project(DS1000Z-ScopeInterface VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(PROJECT_VERSION="${PROJECT_VERSION}")
add_compile_options(-Wall -Wextra -pedantic -Werror)

include_directories(src)
add_executable(
  scope
  main.cpp
  parameters.cpp
  scope.cpp
  workflows.cpp
)

find_package(fmt REQUIRED)
target_link_libraries(scope fmt::fmt) 

set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local")
install(TARGETS scope DESTINATION bin)
