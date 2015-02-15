#ifndef ABSTRACT_LAYER_H
#define ABSTRACT_LAYER_H

#include <deque>
#include <mutex>

#include "packet.h"
#include "event_loop.h"

class abstract_layer
{
public:
    abstract_layer(bool queue_above = true, bool queue_below = true);
    virtual ~abstract_layer();
    void setAbove(abstract_layer *layer);
    void setBelow(abstract_layer *layer);
    void removeAbove(abstract_layer *layer);
    void removeBelow(abstract_layer *layer);
    virtual void processUp(packet &&data);
    virtual void processDown(packet &&data);

protected:
    abstract_layer *_above = nullptr;
    abstract_layer *_below = nullptr;

    void enable_queue_above(bool enable);
    void enable_queue_below(bool enable);

private:
    bool _enable_queue_above;
    bool _enable_queue_below;
    std::deque<packet> _queue_above;
    std::deque<packet> _queue_below;
    std::mutex _mutex;

    void above_inserted();
    void below_inserted();
};

#endif // ABSTRACT_LAYER_H

