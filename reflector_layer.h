#ifndef REFLECTOR_LAYER_H
#define REFLECTOR_LAYER_H

#include "abstract_layer.h"

class reflector_layer : public abstract_layer
{
public:
    virtual void processUp(packet &&data);
    virtual void processDown(packet &&data);
};

#endif // REFLECTOR_LAYER_H
