#ifndef ABSTRACT_LAYER_H
#define ABSTRACT_LAYER_H

#include "packet.h"
#include "event_loop.h"

class abstract_layer
{
public:
    virtual ~abstract_layer();
    void setAbove(abstract_layer *layer);
    void setBelow(abstract_layer *layer);
    void removeAbove(abstract_layer *layer);
    void removeBelow(abstract_layer *layer);
    virtual void processUp(packet &&data);
    virtual void processDown(packet &&data);
    inline virtual void inserted() { }

protected:
    abstract_layer *_above = nullptr;
    abstract_layer *_below = nullptr;
};

#endif // ABSTRACT_LAYER_H

