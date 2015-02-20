#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <sodium.h>

#include "sodium_secret_layer.h"

sodium_secret_layer::sodium_secret_layer(std::unique_ptr<unsigned char> key)
    : key(std::move(key))
{
    randombytes_buf(nonce_out, crypto_secretbox_NONCEBYTES);
    std::fill_n(nonce_in, crypto_secretbox_NONCEBYTES, 0);
}

sodium_secret_layer::~sodium_secret_layer()
{ }

void sodium_secret_layer::increment(unsigned char nonce[crypto_secretbox_NONCEBYTES])
{
    for (int i = 0; !++nonce[i] && i < crypto_secretbox_NONCEBYTES; ++i);
}

void sodium_secret_layer::processUp(packet &&data)
{
    if (data.size() == 0)
        return;

    byte flags = data.back();
    data.pop_back();

    if (flags & Nonce) {
        if (data.size() < crypto_secretbox_NONCEBYTES) return;
        pop_n_back(data, crypto_secretbox_NONCEBYTES, nonce_in);
    }

    if (flags & Encrypted) {
        if (data.size() < crypto_secretbox_MACBYTES) return;
        std::vector<byte> message(data.size() - crypto_secretbox_MACBYTES);

        if (crypto_secretbox_open_easy(message.data(), data.data(), data.size(), nonce_in, key.get()) < 0) {
            std::cout << "crypto_secretbox_open_easy() failed" << std::endl;
            return;
        }
        increment(nonce_in);

        abstract_layer::processUp(std::move(message));
    } else {
        abstract_layer::processUp(std::move(data));
    }
}

void sodium_secret_layer::processDown(packet &&data)
{
    byte flags = 0;
    packet ciphertext;
    ciphertext.resize(data.size() + crypto_secretbox_MACBYTES);

    increment(nonce_out);
    if (crypto_secretbox_easy(ciphertext.data(), data.data(), data.size(), nonce_out, key.get()) < 0)
        throw std::runtime_error("crypto_secretbox_easy() error");
    flags |= Encrypted;

    if (!syncronized) {
        ciphertext.insert(ciphertext.end(), nonce_out, nonce_out + crypto_secretbox_NONCEBYTES);
        flags |= Nonce;
        syncronized = true;
    }

    ciphertext.push_back(flags);

    ciphertext.receiver_id = data.receiver_id;
    abstract_layer::processDown(std::move(ciphertext));
}

