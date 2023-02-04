cmake_minimum_required(VERSION 3.5)

project(CameraQHYCCD LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

set(SOURCES
    main.cpp
    src/cameraqhyccd.cpp
    src/imageprocess.cpp
    include/cameraqhyccd.h
    include/camstruct.h
    include/camenums.h
    include/imageprocess.h)

add_executable(CameraQHYCCD ${SOURCES})

target_include_directories(CameraQHYCCD PRIVATE "include/" "include/CameraSDK_include" "include/opencv2")

target_link_directories(CameraQHYCCD PRIVATE "lib/" PRIVATE "lib/opencv")

target_link_libraries(CameraQHYCCD PRIVATE qhyccd ${OpenCV_LIBS})
