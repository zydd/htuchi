#ifndef ABSTRACT_LAYER_H
#define ABSTRACT_LAYER_H

#include "packet.h"
#include "event_loop.h"

class abstract_layer
{
public:
    virtual ~abstract_layer();
    virtual void processIn(packet &&data) = 0;
    virtual void processOut(packet &&data) = 0;
    inline virtual void inserted() { }
    void insertAbove(abstract_layer *layer);

protected:
    abstract_layer *_above = nullptr;
    abstract_layer *_below = nullptr;
};

#endif // ABSTRACT_LAYER_H
