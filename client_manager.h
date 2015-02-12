#ifndef CLIENT_MANAGER
#define CLIENT_MANAGER

#include <unordered_map>
#include <vector>
#include <chrono>
#include <functional>
#include <mutex>

#include "abstract_layer.h"
#include "packet.h"

class client_manager : public abstract_layer
{
    using system_clock = std::chrono::system_clock;
    struct client_data {
        system_clock::time_point last_update;
        std::vector<byte> info;
        abstract_layer *above = nullptr;

        client_data() { update_time(); }
        client_data(std::vector<byte> &&info)
        : info(std::move(info))
        { update_time(); }
        void update_time() { last_update = system_clock::now(); }
    };

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
    virtual void processDown(packet &&data);
    virtual void inserted();

    inline virtual void build_above(int id) { }

    void set_info(std::vector<byte> &&info);


protected:
    std::mutex _mutex;
    std::unordered_map<int, client_data> _clients;
    bool _expecting_list = false;
    std::vector<byte> _info;

    std::vector<byte> serialise_list();
};

#endif // CLIENT_MANAGER
