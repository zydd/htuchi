#include "reflector_layer.h"

void reflector_layer::processUp(packet &&data)
{
    if (_below) _below->processDown(std::move(data));
}

void reflector_layer::processDown(packet &&data)
{
    if (_above) _above->processUp(std::move(data));
}
