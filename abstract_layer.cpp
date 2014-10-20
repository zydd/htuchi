#include <system_error>
#include "abstract_layer.h"

abstract_layer::~abstract_layer()
{
    if (_above && _below) {
        _above->_below = _below;
        _below->_above = _above;
    } else if (_above) {
        _above->_below = nullptr;
    } else if (_below) {
        _below->_above = nullptr;
    }
}

void abstract_layer::insertAbove(abstract_layer *layer)
{
    if (layer->_below || (layer->_above && _above))
        throw std::runtime_error("abstract_layer() already inserted");

    if (_above) {
        _above->_below = layer;
        layer->_above = _above;
    }

    layer->_below = this;
    _above = layer;
    layer->inserted();
}

