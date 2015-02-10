#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <string>
#include <memory>

#ifdef PACKET_USE_QT
#include <QVariant>
#include <QDebug>
#endif

using byte = unsigned char;

class packet : public std::vector<byte>
{
public:
    enum { Broadcast = -1 };

    int sender_id = 0;
    int receiver_id = 0;

    inline packet() { }
    packet(const std::vector<byte> &o);
    packet(std::vector<byte> &&o);
    inline packet(const std::string &str) { push(str); }

    template<typename Itr>
    inline packet(Itr begin, Itr end) { push(begin, end); }

// Move capture in lambda needed
//     packet(const packet &) = delete;
//     packet &operator= (const packet &) = delete;

#ifdef PACKET_USE_QT
    inline packet(const QVariant &data) { push(data); }
    inline void push(const QVariant &data) {
        QByteArray array;
        QDataStream out(&array, QIODevice::WriteOnly);
        out << data;
        push(array.begin(), array.end());
    }
#endif

    inline void push(const std::string &str) { push(str.begin(), str.end()); }

    template<typename Itr>
    inline void push(Itr begin, Itr end) { insert(this->end(), begin, end); }
};

#endif // PACKET_H
