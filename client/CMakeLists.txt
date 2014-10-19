find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(Hamatuchi_SRCS
    client.cpp
    mainwindow.cpp
    userdelegate.cpp
    contactsmodel.cpp
    chatwindow.cpp
    ../packet.cpp
    ../abstract_layer.cpp
    ../connection_layer.cpp
    ../sodium_secret_layer.cpp
    ../asio_connection.cpp
    ../asio_acceptor.cpp
    ../event_loop.cpp)

set(Hamatuchi_HDRS mainwindow.h userdelegate.h contactsmodel.h chatwindow.h)
set(Hamatuchi_FORMS mainwindow.ui chatwindow.ui)
set(Hamatuchi_RES res.qrc)

qt4_wrap_cpp(Hamatuchi_HDRS_MOC ${Hamatuchi_HDRS})
qt4_wrap_ui(Hamatuchi_FORMS_HDRS ${Hamatuchi_FORMS})
qt4_add_resources(Hamatuchi_RES_RCC ${Hamatuchi_RES})

add_executable(hamatuchi ${Hamatuchi_SRCS} ${Hamatuchi_HDRS_MOC} ${Hamatuchi_RES_RCC} ${Hamatuchi_FORMS_HDRS})
target_link_libraries(hamatuchi ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} -lsodium)

install(TARGETS hamatuchi RUNTIME DESTINATION bin)
