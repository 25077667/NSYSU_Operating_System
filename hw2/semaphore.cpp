#include "semaphore.hpp"

Semaphore::Semaphore(int count_ = 0) : count(count_) {}

inline void Semaphore::notify()
{
    std::unique_lock<std::mutex> lock(mtx);
    count++;
    cv.notify_one();
}

inline void Semaphore::wait()
{
    std::unique_lock<std::mutex> lock(mtx);
    while (count == 0) {
        cv.wait(lock);
    }
    count--;
}