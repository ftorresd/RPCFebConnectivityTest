
include("${CMAKE_SOURCE_DIR}/extern/root/FindROOT.cmake")

link_directories(${ROOT_LIBRARY_DIR})

set(ROOT_INCLUDE_DIRS ${ROOT_INCLUDE_DIR})
set(ROOT_LIBRARY ${ROOT_LIBRARIES})
