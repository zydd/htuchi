find_package(Qt4 REQUIRED)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(Hamatuchi_HDRS_MOC ../client/chatwindow.h)
qt4_wrap_ui(Hamatuchi_FORMS_HDRS ../client/chatwindow.ui)

add_executable(hamatuchid
               server.cpp
               ../abstract_layer.cpp
               ../event_loop.cpp
               ../crossover_layer.cpp
               ../packet.cpp
               ../basic_connection_layer.cpp
               ../listen_layer.cpp
               ../client/chatwindow.cpp
               ${Hamatuchi_HDRS_MOC} ${Hamatuchi_FORMS_HDRS})

target_link_libraries(hamatuchid ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install(TARGETS hamatuchid RUNTIME DESTINATION bin)
