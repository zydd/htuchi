#include "packet.h"

packet::packet(const std::vector<char> &o)
    : std::vector<char>(o)
{ }


packet::packet(std::vector<char> &&o)
    : std::vector<char>(o)
{ }


void packet::push(const QVariant& data)
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out << data;
    insert(this->begin(), array.begin(), array.end());
}

