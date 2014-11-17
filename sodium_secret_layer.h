#ifndef ENCRYPTION_LAYER_H
#define ENCRYPTION_LAYER_H

#include <sodium.h>

#include "abstract_layer.h"

class sodium_secret_layer : public abstract_layer
{
public:
    sodium_secret_layer(unsigned char *key);
    ~sodium_secret_layer();
    virtual void processIn(packet &&data);
    virtual void processOut(packet &&data);

private:
    enum Flags {
        Encrypted = 1 << 0,
        Nonce     = 1 << 1
    };
    unsigned char *key;
    unsigned char nonce_in[crypto_secretbox_NONCEBYTES];
    unsigned char nonce_out[crypto_secretbox_NONCEBYTES];

    virtual void inserted();
    void increment(unsigned char nonce[crypto_secretbox_NONCEBYTES]);
};

#endif // ENCRYPTION_LAYER_H
