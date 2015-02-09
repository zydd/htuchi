#include <algorithm>
#include "client_manager.h"

client_manager::client_manager()
{

}

void client_manager::processUp(packet &&data)
{
    std::size_t len = data.size();
    if (len == 0)
        return;

    auto client = _clients.find(data.receiver_id);
    if (client == _clients.end())
        _clients.emplace(data.receiver_id, client_data());

    len -= 1;
    byte flags = data[len];
    data.pop_back();

    if ((flags & Info) && len > 0) {
        client->second.update_time();
        client->second.info = std::move(data);
    }

    if (flags & GetList) {
        client->second.update_time();
        packet pack;
        pack.receiver_id = data.sender_id;
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
    }

    if ((flags & Send) && len > 1 + 4) {
        client->second.update_time();
        len -= 1;
        int n_id = data[len];
        len -= n_id * 4;
        if (len > 0) {
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
                processDown(std::move(pack));
            }
        }
    }

    if (flags & LogOut) {
        client->second.update_time();
        _clients.erase(client);
    }
}

