#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <sodium.h>

#include "sodium_secret_layer.h"

sodium_secret_layer::sodium_secret_layer(std::unique_ptr<unsigned char> key)
    : key(std::move(key))
{
    randombytes_buf(nonce_out, crypto_secretbox_KEYBYTES);
    packet pack(nonce_out, nonce_out + crypto_secretbox_NONCEBYTES);
    pack.push_back(Nonce);
    abstract_layer::processDown(std::move(pack));

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
    std::size_t clen = data.size();
    std::size_t mlen = clen - crypto_secretbox_MACBYTES;

    if (clen == 0)
        return;

    clen -= 1;
    byte flags = data[clen];
    data.pop_back();

    if (flags & Nonce) {
        std::cout << "setting nonce" << std::endl;
        if (clen < crypto_secretbox_NONCEBYTES)
            return;
        clen -= crypto_secretbox_NONCEBYTES;
        pop_n_back(data, crypto_secretbox_NONCEBYTES, nonce_in);
    }

    if (flags & Data) {
        std::vector<byte> message(mlen);
        increment(nonce_in);std::cout << "#D: " << (int)*nonce_in << std::endl;

        if (crypto_secretbox_open_easy(message.data(), data.data(), clen, nonce_in, key.get()) < 0) {
            std::cout << "crypto_secretbox_open_easy() failed" << std::endl;
            return;
        }

        abstract_layer::processUp(std::move(message));
    } else {
        abstract_layer::processUp(std::move(data));
    }
}

void sodium_secret_layer::processDown(packet &&data)
{
    std::size_t mlen = data.size();
    std::size_t clen = crypto_secretbox_MACBYTES + mlen;
    unsigned char *message = data.data();
    packet ciphertext;
    ciphertext.resize(clen);

    increment(nonce_out);std::cout << "#E: " << (int)*nonce_out << std::endl;
    if (crypto_secretbox_easy(ciphertext.data(), message, mlen, nonce_out, key.get()) < 0)
        throw std::runtime_error("crypto_secretbox_easy() error");

    ciphertext.push_back(Data);

    ciphertext.receiver_id = data.receiver_id;
    abstract_layer::processDown(std::move(ciphertext));
}

