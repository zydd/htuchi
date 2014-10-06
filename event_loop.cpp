#include "event_loop.h"
#include <QDebug>
event_loop main_event_loop;

event_loop::event_loop()
{
    _task_mutex.lock();
}

void event_loop::post(const std::function<void(void)> &task)
{
    std::lock_guard<std::mutex> lock_guard(_post_mutex);

    _tasks.push_back(task);

    if (_tasks.size() == 1)
        _task_mutex.unlock();
}

void event_loop::run()
{
    while (true) {
        _task_mutex.lock();
        if (_done) return;
        _post_mutex.lock();
        auto task = _tasks.front();
        _tasks.pop_front();
        if (!_tasks.empty())
            _task_mutex.unlock();
        _post_mutex.unlock();
        task();
    }
}

void event_loop::stop()
{
    std::lock_guard<std::mutex> lock_guard(_post_mutex);
    _done = true;
    _task_mutex.unlock();
}
