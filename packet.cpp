#include "packet.h"

packet::packet(const std::vector<byte> &o)
{
    push(o.begin(), o.end());
}

packet::packet(std::vector<byte> &&o)
{
    push(o.begin(), o.end());
}

void packet::push(const QVariant& data)
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out << data;
    push(array.begin(), array.end());
}
