cmake_minimum_required(VERSION 3.5)

project(CameraQHYCCD LANGUAGES CXX)

#execute_process(COMMAND conan install ${CMAKE_SOURCE_DIR} --build=missing myprofile.txt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(CONAN_EXPORTED) # in conan local cache
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
else() # in user space
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
    endif()
    include(${CMAKE_BINARY_DIR}/conan.cmake)
    conan_cmake_run(CONANFILE conanfile.txt BASIC_SETUP CMAKE_TARGETS BUILD missing myprofile.txt)
endif()

#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()

set(SOURCES main.cpp src/cameraqhyccd.cpp include/cameraqhyccd.h include/camstruct.h include/camenums.h)
include_directories("include/" "include/CameraSDK_include")

add_executable(CameraQHYCCD ${SOURCES})

target_include_directories(CameraQHYCCD PRIVATE "include/" "include/CameraSDK_include")
target_link_directories(CameraQHYCCD PRIVATE "lib/")

target_link_libraries(CameraQHYCCD ${CONAN_LIBS} PRIVATE qhyccd)
#target_link_libraries(CameraQHYCCD PRIVATE qhyccd)
