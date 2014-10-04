cmake_minimum_required(VERSION 2.6)
project(hamatuchi)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(Hamatuchi_SRCS Hamatuchi.cpp main.cpp)

qt4_automoc(${Hamatuchi_SRCS})
add_executable(hamatuchi ${Hamatuchi_SRCS})
target_link_libraries(hamatuchi ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install(TARGETS hamatuchi RUNTIME DESTINATION bin)
