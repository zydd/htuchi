#include "ssl_connection.h"

ssl_connection::ssl_connection(asio::io_service &io_service,
                               asio::ssl::context &context)
    : _io_service(io_service),
      _socket(io_service, context)
{

}

void ssl_connection::processIn(packet &&data)
{
    if (_above) _above->processIn(std::move(data));
}

void ssl_connection::processOut(packet &&data)
{
    if (_below) _below->processOut(std::move(data));
}
