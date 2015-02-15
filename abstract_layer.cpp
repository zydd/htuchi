#include "abstract_layer.h"
#include "event_loop.h"

abstract_layer::abstract_layer(bool queue_above, bool queue_below)
    : _enable_queue_above(queue_above),
      _enable_queue_below(queue_below)
{ }


abstract_layer::~abstract_layer()
{ }

void abstract_layer::setAbove(abstract_layer *layer)
{
    _above = layer;
    above_inserted();
}

void abstract_layer::setBelow(abstract_layer *layer)
{
    _below = layer;
    below_inserted();
}

void abstract_layer::removeAbove(abstract_layer* layer)
{
    if (_above == layer) _above = nullptr;
}

void abstract_layer::removeBelow(abstract_layer* layer)
{
    if (_below == layer) _below = nullptr;
}

void abstract_layer::enable_queue_above(bool enable)
{
    _enable_queue_above = enable;
    if (!enable)
        _queue_above.clear();
}

void abstract_layer::enable_queue_below(bool enable)
{
    _enable_queue_below = enable;
    if (!enable)
        _queue_below.clear();
}

void abstract_layer::above_inserted()
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    if (_enable_queue_above) {
        while (!_queue_above.empty()) {
            _above->processUp(std::move(_queue_above.front()));
            _queue_above.pop_front();
        }
    }
}

void abstract_layer::below_inserted()
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    if (_enable_queue_below) {
        while (!_queue_below.empty()) {
            _below->processDown(std::move(_queue_below.front()));
            _queue_below.pop_front();
        }
    }
}


void abstract_layer::processUp(packet &&data)
{
    if (_above) default_event_loop.post([=]() mutable { _above->processUp(std::move(data)); });
    else if (_enable_queue_above) {
        std::lock_guard<std::mutex> lock_guard(_mutex);
        _queue_above.emplace_back(std::move(data));
    }
}

void abstract_layer::processDown(packet &&data)
{
    if (_below) default_event_loop.post([=]() mutable { _below->processDown(std::move(data)); });
    else if (_enable_queue_below) {
        std::lock_guard<std::mutex> lock_guard(_mutex);
        _queue_below.emplace_back(std::move(data));
    }
}

