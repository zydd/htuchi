#include "packet.h"

packet::packet() { }

packet::packet(const std::string &str)
    : packet(str.begin(), str.end())
{ }

packet::packet(const QVariant &data)
    : packet(toByteArray(data))
{ }

packet::packet(const QByteArray &data)
    : packet(data.begin(), data.end())
{ }

packet::packet(std::unique_ptr<char[]> data, const std::size_t &size)
    : _size(size)
{
    _data.emplace_back(std::move(data), size);
}


packet::packet(const char *data, const std::size_t &size)
    : packet(data, data + size)
{ }

template<typename Itr>
packet::packet(Itr begin, Itr end)
    : _size(std::distance(begin, end))
{
    auto data = new char[_size];
    _data.emplace_back(std::move(std::unique_ptr<char[]>(data)), _size);
    std::copy(begin, end, data);
}


packet::~packet()
{ }

QByteArray packet::toByteArray(const QVariant &data)
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out << data;
    return array;
}

char* packet::seriallize() const
{
    auto ret = new char[_size];
    for (auto itr = _data.begin(), end = _data.end(); itr != end; ++itr) {
        int i = 0;
        std::copy(itr->first.get(), itr->first.get() + itr->second, ret + i);
        i += itr->second;
    }
    return ret;
}
