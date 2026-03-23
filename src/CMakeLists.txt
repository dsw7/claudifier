cmake_minimum_required(VERSION 3.18)
project(Claudifier VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 26)

string(TIMESTAMP BUILD_DATE)
string(TIMESTAMP CURRENT_YEAR "%Y")

# Options
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(ENABLE_TESTING "Set the TESTING_ENABLED macro" OFF)

if(ENABLE_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

if(ENABLE_TESTING)
  add_compile_definitions(TESTING_ENABLED)
  set(CMAKE_BUILD_TYPE Debug)
else()
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local/bin")
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

add_compile_definitions(
  BUILD_DATE="${BUILD_DATE}"
  CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
  CURRENT_YEAR="${CURRENT_YEAR}"
  PROJECT_VERSION="${PROJECT_VERSION}"
)

add_compile_options(-Wall -Wextra -pedantic -Werror)

set(SRC_FILES
  api/curl_base.cpp
  api/models.cpp
  api/parse_utils.cpp
  api/query_messages_api.cpp
  api/query_models_api.cpp
  commands/command_models.cpp
  commands/command_run.cpp
  commands/command_utils.cpp
  commands/command_chat.cpp
  main.cpp
)
include_directories(api commands external)

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

add_executable(claudifier ${SRC_FILES})
target_link_libraries(claudifier curl fmt::fmt pthread)
install(TARGETS claudifier DESTINATION ${CMAKE_INSTALL_PREFIX})
