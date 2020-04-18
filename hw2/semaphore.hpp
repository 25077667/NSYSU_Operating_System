#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP
#include <condition_variable>
#include <mutex>
class Semaphore
{
public:
    Semaphore(int);
    inline void notify();
    inline void wait();

private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};
#endif