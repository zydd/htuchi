#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

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
    inline packet(std::vector<byte> const &o) : std::vector<byte>(std::move(o)) { }
    inline packet(std::vector<byte> &&o) : std::vector<byte>(std::move(o)) { }

    template<typename Itr>
    inline packet(Itr begin, Itr end) { insert(this->end(), begin, end); }
};

inline std::vector<byte> &operator<< (std::vector<byte> &lhs, const int rhs)
{
    lhs.push_back((rhs >> 0) & 0xFF);
    lhs.push_back((rhs >> 8) & 0xFF);
    lhs.push_back((rhs >> 16) & 0xFF);
    lhs.push_back((rhs >> 24) & 0xFF);

    return lhs;
}

inline std::vector<byte> &operator>> (std::vector<byte> &lhs, int &rhs)
{
    const std::size_t len = lhs.size();

    if (len < 4) throw std::runtime_error("cannot extract 'int' from 'std::vector<byte>'");

    rhs = lhs[len - 4] << 0
        | lhs[len - 3] << 8
        | lhs[len - 2] << 16
        | lhs[len - 1] << 24;

    lhs.resize(len - 4);

    return lhs;
}

template<typename Itr>
inline void pop_n_back(std::vector<byte> &lhs, const std::size_t n, Itr iterator)
{
    if (n > lhs.size()) throw std::runtime_error("remove_back(): invalid size");

    std::copy_n(lhs.end() - n, n, iterator);
    lhs.resize(lhs.size() - n);
}

#endif // PACKET_H
