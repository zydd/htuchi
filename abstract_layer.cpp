#include "abstract_layer.h"
#include "event_loop.h"

abstract_layer::~abstract_layer()
{ }

void abstract_layer::setAbove(abstract_layer *layer)
{
    _above = layer;
}

void abstract_layer::setBelow(abstract_layer *layer)
{
    _below = layer;
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
    if (_above) default_event_loop.post([=]() mutable { _above->processUp(std::move(data)); });
}

void abstract_layer::processDown(packet &&data)
{
    if (_below) default_event_loop.post([=]() mutable { _below->processDown(std::move(data)); });
}
