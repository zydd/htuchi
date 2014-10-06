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

void abstract_layer::insertAbove(abstract_layer *above)
{
    if (above->_below || (above->_above && _above))
        throw "abstract_layer() trying to insert element that is already on a list";

    if (_above) {
        _above->_below = above;
        above->_above = _above;
    }

    above->_below = this;
    _above = above;
}

void abstract_layer::insertBelow(abstract_layer *below)
{
    if (below->_above || (below->_below && _below))
        throw "abstract_layer() trying to insert element that is already on a list";

    if (_below) {
        _below->_above = below;
        below->_below = _below;
    }

    below->_above = this;
    _below = below;
}
