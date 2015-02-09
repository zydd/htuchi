add_executable(hamatuchid
               server.cpp
               ../abstract_layer.cpp
               ../event_loop.cpp
               ../reflector_layer.cpp
               ../packet.cpp
               ../connection_layer.cpp
               ../asio_acceptor.cpp
               ../asio_connection.cpp
               client_manager.cpp)

target_link_libraries(hamatuchid pthread)

install(TARGETS hamatuchid RUNTIME DESTINATION bin)
