cmake_minimum_required (VERSION 2.6)

include("${CMAKE_SOURCE_DIR}/common.cmake")
include("${CMAKE_SOURCE_DIR}/extern/root/common.cmake")

project (analysis)

# uses root and FebConnectivityTest project
include_directories("${ROOT_INCLUDE_DIRS}")
include_directories("${CMAKE_SOURCE_DIR}/tools/include")
include_directories("${CMAKE_SOURCE_DIR}/hwd/include")
include_directories("${CMAKE_SOURCE_DIR}/FebConnectivityTest/include")

# project settings

add_library(analysis "${CMAKE_CURRENT_SOURCE_DIR}/src/common/TangoColors.cpp")

add_executable(parseid "${CMAKE_CURRENT_SOURCE_DIR}/src/common/parseid.cpp")
target_link_libraries(parseid tools)

add_executable(connectivitytest "${CMAKE_CURRENT_SOURCE_DIR}/src/common/connectivitytest.cpp")
target_link_libraries(connectivitytest analysis tools FebConnectivityTest hwd ${ROOT_LIBRARY})

add_executable(thscan "${CMAKE_CURRENT_SOURCE_DIR}/src/common/thscan.cpp")
target_link_libraries(thscan analysis tools FebConnectivityTest hwd ${ROOT_LIBRARY})

if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/cfg" 
        DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")
endif()
