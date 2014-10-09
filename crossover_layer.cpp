#include "crossover_layer.h"

void crossover_layer::processIn(packet &&data)
{
    if (_above) _above->processOut(std::move(data));
}

void crossover_layer::processOut(packet &&data)
{
    if (_below) _below->processIn(std::move(data));
}
