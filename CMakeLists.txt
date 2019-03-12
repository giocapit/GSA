cmake_minimum_required (VERSION 2.6)
project (GSA)

include_directories (main /home/giovanni/projects/QuantLib)

file(GLOB_RECURSE GSA_FILES "main/*.hpp" "main/*.cpp")

#set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -Wall -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -g -O0 -Wall -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 -O3 -Wall -Wno-deprecated-declarations")
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
#add the executable
add_executable(GSA_calc ${GSA_FILES})

#add the library
target_link_libraries(GSA_calc /home/giovanni/projects/QuantLib/build/ql/libQuantLib.so)
