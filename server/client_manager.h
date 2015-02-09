#ifndef CLIENT_MANAGER
#define CLIENT_MANAGER

#include <unordered_map>
#include <vector>
#include <chrono>

#include "../abstract_layer.h"
#include "../packet.h"

class client_manager : public abstract_layer
{
public:
    client_manager();
    virtual void processUp(packet &&data);

private:
    enum Flags {
        Info    = 1 << 0,
        GetList = 1 << 1,
        Send    = 1 << 2,
        LogOut  = 1 << 6,
        Error   = 1 << 7,
    };
    struct client_data {
        using system_clock = std::chrono::system_clock;

        system_clock::time_point last_update;
        std::vector<byte> info;

        client_data() { update_time(); }
        void update_time() { last_update = system_clock::now(); }
    };
    std::unordered_map<int, client_data> _clients;
};

#endif // CLIENT_MANAGER
