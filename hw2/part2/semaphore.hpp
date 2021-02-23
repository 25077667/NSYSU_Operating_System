#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP
#include <condition_variable>
#include <mutex>
class Semaphore
{
public:
    /* Because there is a "chopstick", hence we use 1 to initialize */
    Semaphore(int count_ = 1) : count(count_) {}
    void notify();
    void wait();

private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};
#endif