#ifndef CLIENT_MANAGER
#define CLIENT_MANAGER

#include <unordered_map>
#include <vector>
#include <chrono>

#include "../abstract_layer.h"
#include "../packet.h"

class client_manager : public abstract_layer
{
    using system_clock = std::chrono::system_clock;

public:
    enum Flags {
        Info    = 1 << 0,
        List    = 1 << 1,
        GetList = 1 << 2,
        Forward = 1 << 3,
        Data    = 1 << 5,
        LogOut  = 1 << 6,
        Error   = 1 << 7,
    };

    client_manager();
    virtual void processUp(packet &&data);

    inline system_clock::time_point last_update() const
    { return _last_update; }


private:
    struct client_data {
        system_clock::time_point last_update;
        std::vector<byte> info;

        client_data() { update_time(); }
        client_data(std::vector<byte> &&info)
            : info(std::move(info))
        { update_time(); }
        void update_time() { last_update = system_clock::now(); }
    };

    std::unordered_map<int, client_data> _clients;
    system_clock::time_point _last_update;
    bool _expecting_list = false;
};

#endif // CLIENT_MANAGER
