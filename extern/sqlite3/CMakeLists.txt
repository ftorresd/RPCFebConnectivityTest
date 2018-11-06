cmake_minimum_required (VERSION 2.6)

include("${CMAKE_SOURCE_DIR}/common.cmake")
include("${CMAKE_SOURCE_DIR}/extern/sqlite3/common.cmake")

project (sqlite3)

if(SQLITE3_LOCAL)

   include(ExternalProject)

   ExternalProject_Add(sqlite3_local
      PREFIX ${CMAKE_BINARY_DIR}/extern
      #URL http://sqlite.org/2013/sqlite-autoconf-3080200.tar.gz
      URL ${CMAKE_CURRENT_SOURCE_DIR}/sqlite-autoconf-3080200.tar.gz
      #URL_HASH SHA1=6033ef603ce221d367c665477514d972ef1dc90e
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${CMAKE_BINARY_DIR} CC=${CMAKE_C_COMPILER}
   )

endif()
