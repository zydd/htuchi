#include <algorithm>
#include "client_manager.h"

client_manager::client_manager()
{
    _last_update = system_clock::now();
}

void client_manager::processUp(packet &&data)
{
    std::size_t len = data.size();
    if (len == 0)
        return;

    auto sender = _clients.find(data.sender_id);
    if (sender == _clients.end()) {
        auto new_client = _clients.emplace(data.sender_id, client_data());
        if (!new_client.second) return;
        sender = new_client.first;
        _last_update = system_clock::now();
    }

    len -= 1;
    byte flags = data[len];
    data.pop_back();

    if (flags & Info) {
        if (len <= 4) return;
        len -= 4;
        int info_size = data[len + 0] << 0
                      | data[len + 1] << 8
                      | data[len + 2] << 16
                      | data[len + 3] << 24;
        len -= info_size;
        if (len <= 0) return;

        sender->second.update_time();
        _last_update = system_clock::now();

        sender->second.info.resize(info_size);
        std::copy_n(data.begin() + len, info_size, sender->second.info.begin());
    }

    if (flags & List) {
        bool expected = _expecting_list;
        _expecting_list = false;

        if (len <= 4) return;
        len -= 4;
        int clients_size = data[len + 0] << 0
                         | data[len + 1] << 8
                         | data[len + 2] << 16
                         | data[len + 3] << 24;

        std::unordered_map<int, client_data> clients;

        while (clients_size-- >= 0) {
            if (len <= 8) return;
            len -= 4;
            int id = data[len + 0] << 0
                   | data[len + 1] << 8
                   | data[len + 2] << 16
                   | data[len + 3] << 24;
            len -= 4;
            int info_size = data[len + 0] << 0
                          | data[len + 1] << 8
                          | data[len + 2] << 16
                          | data[len + 3] << 24;
            len -= info_size;
            if (len <= 0) return;
            std::vector<byte> info(info_size);
            std::copy_n(data.begin() + len, info_size, info.begin());
            clients.emplace(id, std::move(info));
        }

        if (expected)
            _clients = std::move(clients);
    }

    if (flags & GetList) {
        sender->second.update_time();
        _last_update = system_clock::now();
        packet pack;
        pack.receiver_id = data.sender_id;

        int clients_size = _clients.size();
        pack.push_back((clients_size >> 0) & 0xFF);
        pack.push_back((clients_size >> 8) & 0xFF);
        pack.push_back((clients_size >> 16) & 0xFF);
        pack.push_back((clients_size >> 24) & 0xFF);

        for (auto const& client : _clients) {
            int info_size = client.second.info.size();
            if (info_size > 0) {
                int id = client.first;
                pack.push_back((id >> 0) & 0xFF);
                pack.push_back((id >> 8) & 0xFF);
                pack.push_back((id >> 16) & 0xFF);
                pack.push_back((id >> 24) & 0xFF);

                pack.push_back((info_size >> 0) & 0xFF);
                pack.push_back((info_size >> 8) & 0xFF);
                pack.push_back((info_size >> 16) & 0xFF);
                pack.push_back((info_size >> 24) & 0xFF);

                pack.reserve(info_size);
                std::copy_n(client.second.info.begin(), info_size, pack.begin() + pack.size() - info_size);
            }
        }
        abstract_layer::processDown(std::move(pack));
    }

    if (flags & Forward) {
        if (len <= 1 + 4) return;
        sender->second.update_time();
        _last_update = system_clock::now();
        len -= 1;
        int n_id = data[len];
        len -= n_id * 4;
        if (len <= 0) return;
        while (n_id--) {
            packet pack;
            pack.receiver_id = data[len + n_id * 4 + 0] << 0
                             | data[len + n_id * 4 + 1] << 8
                             | data[len + n_id * 4 + 2] << 16
                             | data[len + n_id * 4 + 3] << 24;

            if (pack.receiver_id == data.sender_id)
                continue;

            pack.resize(len);
            std::copy_n(data.begin(), len, pack.begin());
            pack.push_back(Data);
            abstract_layer::processDown(std::move(pack));
        }
    }

    if (flags & Data) {
        if (len <= 0) return;
        abstract_layer *&above = sender->second.above;
        if (above || (above = _allocator(data.sender_id)))
            above->processUp(std::move(data));
    }

    if (flags & LogOut) {
        sender->second.update_time();
        _last_update = system_clock::now();
        _clients.erase(sender);
    }
}

void client_manager::processDown(packet &&data)
{
    int id = data.receiver_id;
    data.push_back((id >> 0) & 0xFF);
    data.push_back((id >> 8) & 0xFF);
    data.push_back((id >> 16) & 0xFF);
    data.push_back((id >> 24) & 0xFF);
    data.push_back(1);
    data.push_back(Forward);
}


void client_manager::inserted()
{
    if (!_info.empty()) {
        packet pack;
        pack.receiver_id = packet::Broadcast;
        int info_size = _info.size();
        pack.push(_info.begin(), _info.end());
        pack.push_back((info_size >> 0) & 0xFF);
        pack.push_back((info_size >> 8) & 0xFF);
        pack.push_back((info_size >> 16) & 0xFF);
        pack.push_back((info_size >> 24) & 0xFF);
        pack.push_back(Info|GetList);
        abstract_layer::processDown(std::move(pack));
        _expecting_list = true;
    }
}

