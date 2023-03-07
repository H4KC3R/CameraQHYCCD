cmake_minimum_required(VERSION 3.5)

project(CameraQHYCCD LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

set(SOURCES
    main.cpp
    src/cameraqhyccd.cpp
    src/imageprocess.cpp
    src/objectivecontroller.cpp
    src/autoexposurehandler.cpp
    src/imageblurmetric.cpp
    src/serialib.cpp
    src/imagepipeline.cpp
    include/cameraqhyccd.h
    include/camstruct.h
    include/camenums.h
    include/imageprocess.h
    include/objectivecontroller.h
    include/autoexposurehandler.h
    include/serialib.h
    include/imageblurmetric.h
    include/imagepipeline.h
)

add_executable(CameraQHYCCD ${SOURCES})

target_include_directories(CameraQHYCCD PRIVATE "include/" "include/CameraSDK_include" ${OpenCV_INCLUDE_DIRS})

target_link_directories(CameraQHYCCD PRIVATE "lib/" )

target_link_libraries(CameraQHYCCD PRIVATE qhyccd PRIVATE ${OpenCV_LIBS})
