#include <algorithm>
#include <iostream>

#include "client_manager.h"
#include "connection_layer.h"

client_manager::client_manager()
{ }

void client_manager::processUp(packet &&data)
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    if (data.empty())
        return;

    auto sender = _clients.find(data.sender_id);
    if (sender == _clients.end()) {
        auto new_client = _clients.emplace(data.sender_id, client_data());
        if (!new_client.second) return;
        sender = new_client.first;
    }

    byte flags = data.back();
    data.pop_back();

    if (flags & Info) {
        if (data.size() <= 4) return;

        int info_size;
        data >> info_size;

        sender->second.update_time();
        sender->second.info.resize(info_size);
        pop_n_back(data, info_size, sender->second.info.begin());

        for (auto const& client : _clients) {
            packet pack = serialise_list();
            pack.receiver_id = client.first;
            pack.push_back(List);
            abstract_layer::processDown(std::move(pack));
        }
        std::cout << "client_manager::Info" << std::endl;
    }

    if (flags & List) {
        if (data.size() <= 4) return;

        int clients_size;
        data >> clients_size;

        std::unordered_map<int, client_data> clients;

        while (clients_size--) {
            if (data.size() <= 8) return;

            int id;
            data >> id;

            int info_size;
            data >> info_size;

            std::vector<byte> info(info_size);
            pop_n_back(data, info_size, info.begin());
            clients.emplace(id, std::move(info));
        }

        _clients = std::move(clients);
        std::cout << "client_manager::List" << std::endl;
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
        if (data.size() <= 1 + 4) return;
        sender->second.update_time();
        int n_id = data.back();
        data.pop_back();
        int data_len = data.size() - 4 * n_id;
        if (data_len <= 0) return;
        while (n_id--) {
            packet pack;
            data >> pack.receiver_id;

//             if (pack.receiver_id == data.sender_id)
//                 continue;

            pack.resize(data_len);
            std::copy_n(data.begin(), data_len, pack.begin());
            pack << sender->first;
            pack.push_back(Data);
            abstract_layer::processDown(std::move(pack));
        }
        std::cout << "client_manager::Forward" << std::endl;
    }

    if (flags & Data) {
        if (data.size() <= 4) return;
        int sender_id;
        data >> sender_id;

        auto client = _clients.find(sender_id);
        if (client != _clients.end()) {
            abstract_layer *&above = client->second.above;
            if (!above) {
                build_above(sender_id);
                if (above) above->setBelow(this);
            }
            if (above) {
                above->processUp(std::move(data));
                std::cout << "client_manager::Data" << std::endl;
            }
        }
    }
}

void client_manager::processDown(packet &&data)
{
    data << data.receiver_id;
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
        pack << _info.size();
        pack.push_back(Info|GetList);
        abstract_layer::processDown(std::move(pack));
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

            pack << info_size
                 << client.first;
        }
    }

    pack << _clients.size();

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

