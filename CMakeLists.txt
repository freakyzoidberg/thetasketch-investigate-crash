cmake_minimum_required(VERSION 2.8.9)
project (theta-client-1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/thirdparty/incubator-datasketches-cpp-1.0.0-incubating/theta/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/incubator-datasketches-cpp-1.0.0-incubating/common/include)

file(GLOB SOURCES "src/*.cpp")
file(GLOB LIBRARIES "thirdparty/build/*.dylib")
message("LIBRARIES = ${LIBRARIES}")

add_executable(theta-client-1.0.0 ${SOURCES})
target_link_libraries(theta-client-1.0.0 ${LIBRARIES})  
