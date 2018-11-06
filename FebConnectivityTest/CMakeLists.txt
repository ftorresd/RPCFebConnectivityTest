cmake_minimum_required (VERSION 2.6)

include("${CMAKE_SOURCE_DIR}/common.cmake")

project (FebConnectivityTest)

add_definitions(-DOFFLINE_ANALYSIS)

# uses log4cplus, hwd, tools project
include_directories("${CMAKE_SOURCE_DIR}/log4cplus/include")
include_directories("${CMAKE_SOURCE_DIR}/hwd/include")
include_directories("${CMAKE_SOURCE_DIR}/tools/include")

# project settings

file(GLOB CPP_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/*.cpp"
)

add_library(FebConnectivityTest ${CPP_SRC})
target_link_libraries(FebConnectivityTest tools hwd log4cplus.so)
