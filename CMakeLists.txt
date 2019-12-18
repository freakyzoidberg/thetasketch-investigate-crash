cmake_minimum_required(VERSION 2.8.9)
project (theta-client-1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/*.cpp")

set(THETA_DIR incubator-datasketches-cpp)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/${THETA_DIR}/theta/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/${THETA_DIR}/common/include)
file(GLOB LIBRARIES "thirdparty/${THETA_DIR}/build/*.dylib")



message("LIBRARIES = ${LIBRARIES}")

add_executable(theta-client-1.0.0 ${SOURCES})
target_link_libraries(theta-client-1.0.0 ${LIBRARIES})  
