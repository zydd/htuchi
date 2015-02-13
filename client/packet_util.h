#ifndef PACKET_UTIL_H
#define PACKET_UTIL_H

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QVariant>

#include "../packet.h"

inline packet toPacket(QVariant const& data)
{
    QByteArray array;
    QDataStream ds(&array, QIODevice::WriteOnly);
    ds << data;
    return packet(array.begin(), array.end());
}

inline QVariant toVariant(packet const& data)
{
    QByteArray array((char *)data.data(), data.size());
    QDataStream ds(array);
    QVariant var;
    ds >> var;
    return var;
}

#endif // PACKET_UTIL_H
