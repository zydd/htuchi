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

    auto client = _clients.find(data.receiver_id);
    if (client == _clients.end()) {
        auto new_client = _clients.emplace(data.receiver_id, client_data());
        if (!new_client.second) return;
        client = new_client.first;
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

        client->second.update_time();
        _last_update = system_clock::now();

        client->second.info.resize(info_size);
        std::copy_n(data.begin() + len, info_size, client->second.info.begin());
    }

    if (flags & List) {
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

        _clients = std::move(clients);
    }

    if (flags & GetList) {
        client->second.update_time();
        _last_update = system_clock::now();
        packet pack;
        pack.receiver_id = data.sender_id;

        int clients_size = _clients.size();
        pack.push_back((clients_size >> 0) & 0xFF);
        pack.push_back((clients_size >> 8) & 0xFF);
        pack.push_back((clients_size >> 16) & 0xFF);
        pack.push_back((clients_size >> 24) & 0xFF);

        for (auto client : _clients) {
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
        client->second.update_time();
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
            abstract_layer::processDown(std::move(pack));
        }
    }

    if (flags & Data) {
        if (len <= 0) return;
        abstract_layer::processUp(std::move(data));
    }

    if (flags & LogOut) {
        client->second.update_time();
        _last_update = system_clock::now();
        _clients.erase(client);
    }
}

