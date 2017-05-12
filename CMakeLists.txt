cmake_minimum_required(VERSION 2.6)
PROJECT(RTK2CSV)

FIND_PACKAGE(Qt5 COMPONENTS Gui Widgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions ("-Wall -Wunused -std=c++11")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(EXPOSERGUI_PART_UI
    src/window.ui
)

set(EXPOSERGUI_PART_SRCS
    src/main.cpp
    src/window.cpp
)

qt5_wrap_ui(EXPOSERGUI_PART_SRCS ${EXPOSERGUI_PART_UI})
ADD_EXECUTABLE(RTK2CSV ${EXPOSERGUI_PART_SRCS})
TARGET_LINK_LIBRARIES(RTK2CSV
    Qt5::Gui
    Qt5::Widgets
)
