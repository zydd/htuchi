#include <algorithm>
#include <iostream>

#include "client_manager.h"
#include "connection_layer.h"

client_manager::client_manager()
{ }

void client_manager::processUp(packet &&data)
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    int len = data.size();
    if (len == 0)
        return;

    auto sender = _clients.find(data.sender_id);
    if (sender == _clients.end()) {
        auto new_client = _clients.emplace(data.sender_id, client_data());
        if (!new_client.second) return;
        sender = new_client.first;
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
        if (len < 0) return;

        sender->second.update_time();

        sender->second.info.resize(info_size);
        std::copy_n(data.begin() + len, info_size, sender->second.info.begin());

        for (auto const& client : _clients) {
            packet pack = serialise_list();
            pack.receiver_id = client.first;
            pack.push_back(List);
            abstract_layer::processDown(std::move(pack));
        }
        std::cout << "client_manager::Info" << std::endl;
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

        while (clients_size--) {
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
            if (len < 0) return;
            std::vector<byte> info(info_size);
            std::copy_n(data.begin() + len, info_size, info.begin());
            clients.emplace(id, std::move(info));
        }

        /*if (expected)*/ {
            _clients = std::move(clients);
            std::cout << "client_manager::List" << std::endl;
        }
    }

    if (flags & GetList) {
//         sender->second.update_time();
//         _last_update = system_clock::now();
//         packet pack = serialise_list();
//         pack.receiver_id = data.sender_id;
//
//         pack.push_back(List);
//
//         abstract_layer::processDown(std::move(pack));
//
//         std::cout << "client_manager::GetList\n";
    }

    if (flags & Forward) {
        if (len <= 1 + 4) return;
        sender->second.update_time();
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

//             if (pack.receiver_id == data.sender_id)
//                 continue;

            pack.resize(len);
            std::copy_n(data.begin(), len, pack.begin());
            int id = sender->first;
            pack.push_back((id >> 0) & 0xFF);
            pack.push_back((id >> 8) & 0xFF);
            pack.push_back((id >> 16) & 0xFF);
            pack.push_back((id >> 24) & 0xFF);
            pack.push_back(Data);
            abstract_layer::processDown(std::move(pack));
        }
        std::cout << "client_manager::Forward" << std::endl;
    }

    if (flags & Data) {
        if (len <= 4) return;
        len -= 4;
        int sender_id = data[len + 0] << 0
                      | data[len + 1] << 8
                      | data[len + 2] << 16
                      | data[len + 3] << 24;

        auto client = _clients.find(sender_id);
        if (client != _clients.end()) {
            abstract_layer *&above = client->second.above;
            if (!above) {
                build_above(sender_id);
                if (above) above->setBelow(this);
            }
            if (above) {
                data.resize(len);
                above->processUp(std::move(data));
                std::cout << "client_manager::Data" << std::endl;
            }
        }
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
    data.receiver_id = packet::Broadcast;
    abstract_layer::processDown(std::move(data));
}


void client_manager::inserted()
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    if (!_info.empty()) {
        packet pack(_info);
        pack.receiver_id = packet::Broadcast;
        int info_size = _info.size();
        pack.push_back((info_size >> 0) & 0xFF);
        pack.push_back((info_size >> 8) & 0xFF);
        pack.push_back((info_size >> 16) & 0xFF);
        pack.push_back((info_size >> 24) & 0xFF);
        pack.push_back(Info|GetList);
        abstract_layer::processDown(std::move(pack));
        _expecting_list = true;
    }
}

void client_manager::set_info(std::vector<byte>&& info)
{
    _mutex.lock();
    _info = std::move(info);
    _mutex.unlock();
    inserted();
}

std::vector<byte> client_manager::serialise_list()
{
    std::vector<byte> pack;
    for (auto const& client : _clients) {
        int info_size = client.second.info.size();
        if (info_size > 0) {
            pack.resize(pack.size() + info_size);
            std::copy_n(client.second.info.begin(), info_size, pack.begin() + pack.size() - info_size);

            pack.push_back((info_size >> 0) & 0xFF);
            pack.push_back((info_size >> 8) & 0xFF);
            pack.push_back((info_size >> 16) & 0xFF);
            pack.push_back((info_size >> 24) & 0xFF);

            int id = client.first;
            pack.push_back((id >> 0) & 0xFF);
            pack.push_back((id >> 8) & 0xFF);
            pack.push_back((id >> 16) & 0xFF);
            pack.push_back((id >> 24) & 0xFF);
        }
    }

    int clients_size = _clients.size();
    pack.push_back((clients_size >> 0) & 0xFF);
    pack.push_back((clients_size >> 8) & 0xFF);
    pack.push_back((clients_size >> 16) & 0xFF);
    pack.push_back((clients_size >> 24) & 0xFF);

    return pack;
}

void client_manager::update_client_status(int id, int status)
{
    if (status == connection_layer::Offline) {
        auto client = _clients.find(id);
        if (client != _clients.end()) {
            delete client->second.above;

            _clients.erase(client);
            for (auto const& client : _clients) {
                packet pack = serialise_list();
                pack.receiver_id = client.first;
                pack.push_back(List);
                abstract_layer::processDown(std::move(pack));
            }
        }
    }
}

