#ifndef REFLECTOR_LAYER_H
#define REFLECTOR_LAYER_H

#include "abstract_layer.h"

class reflector_layer : public abstract_layer
{
public:
    virtual void processIn(packet &&data);
    virtual void processOut(packet &&data);
};

#endif // REFLECTOR_LAYER_H
