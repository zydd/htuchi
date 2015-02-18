#ifndef ENCRYPTION_LAYER_H
#define ENCRYPTION_LAYER_H

#include <memory>
#include <sodium.h>

#include "abstract_layer.h"

class sodium_secret_layer : public abstract_layer
{
public:
    sodium_secret_layer(std::unique_ptr<unsigned char> key);
    ~sodium_secret_layer();
    virtual void processUp(packet &&data);
    virtual void processDown(packet &&data);

private:
    enum Flags {
        Data  = 1 << 0,
        Nonce = 1 << 1
    };
    std::unique_ptr<unsigned char> key;
    unsigned char nonce_in[crypto_secretbox_NONCEBYTES];
    unsigned char nonce_out[crypto_secretbox_NONCEBYTES];

    void increment(unsigned char nonce[crypto_secretbox_NONCEBYTES]);
};

#endif // ENCRYPTION_LAYER_H
