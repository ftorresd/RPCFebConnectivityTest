cmake_minimum_required (VERSION 2.6)

include("${CMAKE_SOURCE_DIR}/common.cmake")
include("${CMAKE_SOURCE_DIR}/extern/log4cplus/common.cmake")

project (log4cplus)

if(LOG4CPLUS_LOCAL)

   include(ExternalProject)

   ExternalProject_Add(log4cplus_local
      PREFIX ${CMAKE_BINARY_DIR}/extern
      #URL http://sourceforge.net/projects/log4cplus/files/log4cplus-stable/1.1.2/log4cplus-1.1.2.tar.gz/download
      URL ${CMAKE_CURRENT_SOURCE_DIR}/log4cplus-1.1.2.tar.gz
      #URL_HASH SHA1=a17d4a158cf47b734720bc5916553a10f9174d80
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR} -DCMAKE_C_COMPILER:PATH=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER:PATH=${CMAKE_CXX_COMPILER}
   )

endif()
