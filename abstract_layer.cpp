#include <system_error>
#include "abstract_layer.h"

abstract_layer::~abstract_layer()
{ }

void abstract_layer::setAbove(abstract_layer *layer)
{
    _above = layer;
    layer->inserted();
}

void abstract_layer::setBelow(abstract_layer *layer)
{
    _below = layer;
    layer->inserted();
}

void abstract_layer::removeAbove(abstract_layer* layer)
{
    if (_above == layer) _above = nullptr;
}

void abstract_layer::removeBelow(abstract_layer* layer)
{
    if (_below == layer) _below = nullptr;
}


void abstract_layer::processUp(packet &&data)
{
    if (_above) _above->processUp(std::move(data));
}

void abstract_layer::processDown(packet &&data)
{
    if (_below) _below->processDown(std::move(data));
}
