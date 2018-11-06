cmake_minimum_required (VERSION 2.6)

include("${CMAKE_SOURCE_DIR}/common.cmake")

project (tools)

# project settings
file(GLOB CPP_SRC
     "${CMAKE_CURRENT_SOURCE_DIR}/src/common/*.cpp"
)
#file(GLOB CPP_SRC
#    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/*.cpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/*/*.cpp"
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/common/Time.cpp"
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/common/Timer.cpp"
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/common/Chrono.cpp"
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/common/Date.cpp"
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/common/RollId.cpp"
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/common/RollSelection.cpp"
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/common/RPCDetId.cpp"
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/common/Murmur3a.cpp"
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/common/tools.cpp"
#)

add_library(tools ${CPP_SRC})
