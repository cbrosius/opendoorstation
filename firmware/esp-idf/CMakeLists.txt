# This is the top-level CMakeLists.txt file for the project.

cmake_minimum_required(VERSION 3.16)

# Load environment variables from .env file if it exists
if(EXISTS ${CMAKE_SOURCE_DIR}/../../.env)
    file(STRINGS ${CMAKE_SOURCE_DIR}/../../.env env_vars)
    foreach(var ${env_vars})
        # Regex to parse KEY="VALUE" or KEY=VALUE
        if(var MATCHES "^([A-Za-z0-9_]+)=['\"]?([^'\"]*)['\"]?$")
            set(ENV{${CMAKE_MATCH_1}} "${CMAKE_MATCH_2}")
            # Define them for C code
            add_compile_definitions(${CMAKE_MATCH_1}="${CMAKE_MATCH_2}")
        endif()
    endforeach()
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(IDF_TARGET esp32s3)

project(opendoorstation)
