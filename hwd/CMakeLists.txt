cmake_minimum_required (VERSION 2.6)

include("${CMAKE_SOURCE_DIR}/common.cmake")
include("${CMAKE_SOURCE_DIR}/extern/log4cplus/common.cmake")
include("${CMAKE_SOURCE_DIR}/extern/sqlite3/common.cmake")

project (hwd)

add_definitions(-DOFFLINE_ANALYSIS)

# uses log4cplus, sqlite3 and tools project
include_directories("${LOG4CPLUS_INCLUDE_DIRS}")
include_directories("${SQLITE3_INCLUDE_DIRS}")
include_directories("${CMAKE_SOURCE_DIR}/tools/include")

# project settings
file(GLOB CPP_SRC
     "${CMAKE_CURRENT_SOURCE_DIR}/src/common/*.cpp"
)

add_library(hwd ${CPP_SRC})
target_link_libraries(hwd tools ${LOG4CPLUS_LIBRARY} ${SQLITE3_LIBRARY})

if (SQLITE3_LOCAL)
   add_dependencies(hwd sqlite3_local)
endif()

if (LOG4CPLUS_LOCAL)
   add_dependencies(hwd log4cplus_local)
endif()
