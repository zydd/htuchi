find_package(Qt4 REQUIRED)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(hamatuchid
               server.cpp
               ../abstract_layer.cpp
               ../event_loop.cpp
               ../reflector_layer.cpp
               ../packet.cpp
               ../connection_layer.cpp
               ../asio_acceptor.cpp
               ../asio_connection.cpp
               ../ssl_connection.cpp)

target_link_libraries(hamatuchid ${QT_QTCORE_LIBRARY} -lssl -lcrypto)

install(TARGETS hamatuchid RUNTIME DESTINATION bin)
