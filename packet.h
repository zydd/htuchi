#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <string>
#include <memory>
#include <QVariant>
#include <QDebug>

class packet : public std::vector<char>
{
public:
    using std::vector<char>::iterator;
    using std::vector<char>::const_iterator;
    using std::vector<char>::begin;
    using std::vector<char>::data;
    using std::vector<char>::end;
    using std::vector<char>::size;
    using std::vector<char>::operator[];

    int id = -1;

    inline packet() { }
    packet(const std::vector<char> &o);
    packet(std::vector<char> &&o);
    inline packet(const std::string &str) { push(str); }
    inline packet(const QVariant &data) { push(data); }

    template<typename Itr>
    packet(Itr begin, Itr end) { insert(this->end(), begin, end); }

// Move capture in lambda needed
//     packet(const packet &) = delete;
//     packet &operator= (const packet &) = delete;

    void push(const QVariant &data);

    inline void push(const std::string &str) { insert(end(), str.begin(), str.end()); }

    template<typename Itr>
    void push(Itr begin, Itr end) { insert(this->end(), begin, end); }

};

#endif // PACKET_H
