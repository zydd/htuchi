#include "packet.h"

packet::packet() { }

packet::packet(const std::string &str)
    : std::vector<char>(str.begin(), str.end())
{ }

packet::packet(const QVariant &data)
    : packet(toByteArray(data))
{ }

packet::packet(const QByteArray &data)
    : std::vector<char>(data.begin(), data.end())
{ }

packet::packet(const char *data, const std::size_t &size)
    : std::vector<char>(data, data + size)
{ }

packet::~packet()
{ }

QByteArray packet::toByteArray(const QVariant &data)
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out << data;
    return array;
}
