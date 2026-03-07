cmake_minimum_required(VERSION 3.18)
project(Claudifier VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local/bin")

string(TIMESTAMP BUILD_DATE)
string(TIMESTAMP CURRENT_YEAR "%Y")

add_compile_options(-Wall -Wextra -pedantic -Werror)
add_compile_definitions(
  BUILD_DATE="${BUILD_DATE}"
  CURRENT_YEAR="${CURRENT_YEAR}"
  PROJECT_VERSION="${PROJECT_VERSION}"
)

set(SRC_FILES
  main.cpp
  networking.cpp
)

add_executable(cl ${SRC_FILES})
install(TARGETS cl DESTINATION ${CMAKE_INSTALL_PREFIX})
