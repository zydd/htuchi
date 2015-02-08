#include <random>
#include <stdexcept>
#include <cstring>
#include <sodium.h>

#include "sodium_secret_layer.h"

sodium_secret_layer::sodium_secret_layer(unsigned char *key)
    : key(key)
{
    randombytes_buf(nonce_out, crypto_secretbox_KEYBYTES);
    std::memset(nonce_in, 0, crypto_secretbox_NONCEBYTES);
}

sodium_secret_layer::~sodium_secret_layer()
{ }

void sodium_secret_layer::increment(unsigned char nonce[crypto_secretbox_NONCEBYTES])
{
    int i = 0;
    while (++nonce[i] == 0 && i < crypto_secretbox_NONCEBYTES) ++i;
}


void sodium_secret_layer::processUp(packet &&data)
{
    if (!_above) return;

    std::size_t clen = data.size();
    std::size_t mlen = clen - crypto_secretbox_MACBYTES;
    unsigned char *ciphertext = data.data();

    if (clen == 0)
        return;

    clen -= 1;
    byte flags = data[clen];
    data.pop_back();

    if (flags & Nonce) {
        if (clen < crypto_secretbox_NONCEBYTES)
            return;
        clen -= crypto_secretbox_NONCEBYTES;
        memcpy(nonce_in, ciphertext + clen, crypto_secretbox_NONCEBYTES);
    }

    if (flags & Encrypted) {
        std::vector<byte> message(mlen);
        increment(nonce_in);

        if (crypto_secretbox_open_easy(message.data(), ciphertext, clen, nonce_in, key) < 0) {
//             qDebug() << "decryption failed";
            return;
        }

        _above->processUp(std::move(message));
    } else
        _above->processUp(std::move(data));
}

void sodium_secret_layer::processDown(packet &&data)
{
    if (!_below) return;

    std::size_t mlen = data.size();
    std::size_t clen = crypto_secretbox_MACBYTES + mlen;
    unsigned char *message = data.data();
    std::vector<byte> ciphertext(clen);

    increment(nonce_out);
    if (crypto_secretbox_easy(ciphertext.data(), message, mlen, nonce_out, key) < 0)
        throw std::runtime_error("crypto_secretbox_easy() error");

    packet pack(std::move(ciphertext));
    pack.push(nonce_out, nonce_out + crypto_secretbox_NONCEBYTES);
    pack.push_back(Encrypted | Nonce);

    _below->processDown(std::move(pack));
}

void sodium_secret_layer::inserted()
{
    if (!_below) return;
    packet pack(nonce_out, nonce_out + crypto_secretbox_NONCEBYTES);
    pack.push_back(Nonce);
    _below->processDown(std::move(pack));
}

