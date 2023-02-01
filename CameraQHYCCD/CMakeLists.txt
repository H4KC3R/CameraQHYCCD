cmake_minimum_required(VERSION 3.5)

project(CameraQHYCCD LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(SOURCES main.cpp src/cameraqhyccd.cpp include/cameraqhyccd.h include/camstruct.h include/camenums.h)
include_directories("include/" "include/CameraSDK_include")

add_executable(CameraQHYCCD ${SOURCES})

target_include_directories(CameraQHYCCD PRIVATE "include/" "include/CameraSDK_include")
target_link_directories(CameraQHYCCD PRIVATE "lib/")

target_link_libraries(CameraQHYCCD ${CONAN_LIBS} PRIVATE qhyccd)
