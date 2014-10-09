#include "reflector_layer.h"

void reflector_layer::processIn(packet &&data)
{
    data.id = -1;
    if (_below) _below->processOut(std::move(data));
}

void reflector_layer::processOut(packet &&data)
{
    if (_above) _above->processIn(std::move(data));
}
