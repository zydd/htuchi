#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <deque>
#include <mutex>
#include <functional>

class event_loop
{
public:
    event_loop();
    void post(const std::function<void(void)> &task);
    void stop();
    void run();

private:
    bool _done = false;
    std::mutex _task_mutex;
    std::mutex _post_mutex;
    std::deque<std::function<void(void)>> _tasks;
};

extern event_loop default_event_loop;

#endif // EVENT_LOOP_H
