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
  api/api.cpp
  api/create_message.cpp
  api/parse_error.cpp
  main.cpp
)
include_directories(api external)

# -----------------------------------------------------------------------------------------------------------
# External dependencies
message(STATUS "Locating {fmt} library")
find_package(fmt)

if(NOT fmt_FOUND)
  message(FATAL_ERROR "Is the {fmt} package installed?")
endif()

# nlohmann/json.hpp
message(STATUS "Checking for nlohmann/json.hpp")

set(NLOHMANN_JSON_HPP_DL "${CMAKE_SOURCE_DIR}/external/json.hpp")
if(EXISTS ${NLOHMANN_JSON_HPP_DL})
  message(STATUS "${NLOHMANN_JSON_HPP_DL} already exists. Skipping download")
else()
  message(STATUS "Downloading nlohmann/json.hpp")
  file(
    DOWNLOAD
    "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"
    ${NLOHMANN_JSON_HPP_DL}
    SHOW_PROGRESS
  )
endif()
# -----------------------------------------------------------------------------------------------------------

add_executable(cl ${SRC_FILES})
target_link_libraries(cl curl fmt::fmt)
install(TARGETS cl DESTINATION ${CMAKE_INSTALL_PREFIX})
