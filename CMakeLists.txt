cmake_minimum_required (VERSION 2.6)

include("${CMAKE_SOURCE_DIR}/common.cmake")

# Default to build type Release
# to change, pass (for example) -DCMAKE_BUILD_TYPE=Debug
if(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
endif()

project (RPCFebConnectivityTest)

add_subdirectory(extern/sqlite3)
add_subdirectory(extern/log4cplus)
add_subdirectory(tools)
add_subdirectory(hwd)
add_subdirectory(FebConnectivityTest)
add_subdirectory(analysis)
