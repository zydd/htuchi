#ifndef CROSSOVER_LAYER_H
#define CROSSOVER_LAYER_H

#include "abstract_layer.h"

class crossover_layer : public abstract_layer
{
public:
    virtual void processIn(packet &&data);
    virtual void processOut(packet &&data);
};

#endif // CROSSOVER_LAYER_H
