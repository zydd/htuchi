#include "packet.h"

packet::packet(const std::vector<byte> &o)
{
    push(o.begin(), o.end());
}

packet::packet(std::vector<byte> &&o)
{
    push(o.begin(), o.end());
}

