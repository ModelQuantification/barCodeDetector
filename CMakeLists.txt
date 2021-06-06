cmake_minimum_required(VERSION 3.5)

project(barCodeDetector LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}
                      ${GITHUB_WORKSPACE}/qtApp/Qt/5.12.10/5.12.10/gcc_64
                      /home/hi/Qt5.12.10/5.12.10/gcc_64)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)
find_package(PythonLibs 3.6 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

include_directories(User)
file(GLOB_RECURSE SOURCES "User/*.*")

qt5_add_resources(qrcs ico.qrc)

add_executable(barCodeDetector
  ${SOURCES}
  ${qrcs}
  main.cpp
  widget.cpp
  widget.h
  widget.ui
)

target_link_libraries(barCodeDetector PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${OpenCV_LIBS} ${PYTHON_LIBRARIES})
