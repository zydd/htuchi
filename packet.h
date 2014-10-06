#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <string>
#include <QVariant>
#include <QDebug>

class packet : public std::vector<char>
{
public:
    packet();
    packet(const std::string &str);
    packet(const QVariant &data);
    packet(const char *data, const std::size_t &size);
//    packet(const packet&) = delete;
//    packet(const packet &&other) noexcept;
    ~packet();

private:
    packet(const QByteArray &data);
    static QByteArray toByteArray(const QVariant &data);
};

#endif // PACKET_H
