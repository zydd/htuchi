#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <string>
#include <memory>
#include <QVariant>
#include <QDebug>

using byte = unsigned char;

class packet : public std::vector<byte>
{
public:
    inline packet() { }
    packet(const std::vector<byte> &o);
    packet(std::vector<byte> &&o);
    inline packet(const std::string &str) { push(str); }
    inline packet(const QVariant &data) { push(data); }

    template<typename Itr>
    inline packet(Itr begin, Itr end) { push(begin, end); }

// Move capture in lambda needed
//     packet(const packet &) = delete;
//     packet &operator= (const packet &) = delete;

    void push(const QVariant &data);

    inline void push(const std::string &str) { push(str.begin(), str.end()); }

    template<typename Itr>
    inline void push(Itr begin, Itr end) { insert(this->end(), begin, end); }
};

#endif // PACKET_H
