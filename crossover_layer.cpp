#include "crossover_layer.h"

void crossover_layer::processIn(const packet &data)
{
    if (_above) _above->processOut(data);
}

void crossover_layer::processOut(const packet &data)
{
    if (_below) _below->processIn(data);
}
