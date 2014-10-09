#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <string>
#include <memory>
#include <QVariant>
#include <QDebug>

class packet
{
public:
    int id = -1;

    packet();
    packet(const std::string &str);
    packet(const QVariant &data);
    packet(const char *data, const std::size_t &size);
    packet(std::unique_ptr<char[]> data, const std::size_t &size);
    template<typename Itr>
    packet(Itr begin, Itr end);
    ~packet();

    char *seriallize() const;
    inline std::size_t size() const {
        std::size_t size = 0;
        for (auto itr = _data.begin(), end = _data.end(); itr != end; ++itr)
            size += itr->second;
        return size;
    }

private:
    std::vector<std::pair<std::unique_ptr<char[]>, std::size_t>> _data;

    packet(const QByteArray &data);
    static QByteArray toByteArray(const QVariant &data);
};

#endif // PACKET_H
