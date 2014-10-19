#include <random>
#include <sodium.h>

#include "sodium_secret_layer.h"

sodium_secret_layer::sodium_secret_layer(unsigned char *key)
    : key(key)
{
    randombytes_buf(nonce_out, crypto_secretbox_KEYBYTES);
    memset(nonce_in, 0, crypto_secretbox_NONCEBYTES);
}

sodium_secret_layer::~sodium_secret_layer()
{ }

void sodium_secret_layer::increment(unsigned char nonce[crypto_secretbox_NONCEBYTES])
{
    int carry = 0;
    int i = 0;
    while (++nonce[i] == 0 && i < crypto_secretbox_NONCEBYTES) ++i;
}


void sodium_secret_layer::processIn(packet &&data)
{
    if (!_above) return;

    std::size_t clen = data.size();
    std::size_t mlen = clen - crypto_secretbox_MACBYTES;
    unsigned char *ciphertext = (unsigned char *) data.data();
    std::vector<char> message(mlen);

    if (!sync_in) {
        if (clen < crypto_secretbox_NONCEBYTES)
            return;
        clen -= crypto_secretbox_NONCEBYTES;
        memcpy(nonce_in, ciphertext + clen, crypto_secretbox_NONCEBYTES);
        sync_in = true;
    }

    if (crypto_secretbox_open_easy((unsigned char *) message.data(), ciphertext, clen, nonce_in, key) < 0) {
        qDebug() << "decryption failed";
        return;
    }

    _above->processIn(std::move(message));
    increment(nonce_in);
}

void sodium_secret_layer::processOut(packet &&data)
{
    if (!_below) return;

    std::size_t mlen = data.size();
    std::size_t clen = crypto_secretbox_MACBYTES + mlen;
    unsigned char *message = (unsigned char *) data.data();
    std::vector<char> ciphertext(clen);

    if (crypto_secretbox_easy((unsigned char *) ciphertext.data(), message, mlen, nonce_out, key) < 0)
        throw std::runtime_error("crypto_secretbox_easy() error");

    packet pack(std::move(ciphertext));
    if (!sync_out) {
        pack.push(nonce_out, nonce_out + crypto_secretbox_NONCEBYTES);
        sync_out = true;
    }

    _below->processOut(std::move(pack));
    increment(nonce_out);
}

