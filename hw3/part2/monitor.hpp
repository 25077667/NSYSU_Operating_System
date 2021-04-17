#ifndef __MONITOR_HPP__
#define __MONITOR_HPP__
#include <mutex>
#define NUM 5

class Monitor
{
    std::array<std::mutex, NUM> arr;

public:
    Monitor() = default;
    ~Monitor() = default;

    // --------- Atomics ---------

    // lock a chopstick, others wait(others will be context switch)
    void lock(int id);
    // unlock a chopstick, wake up others
    void unlock(int id);
};

#endif