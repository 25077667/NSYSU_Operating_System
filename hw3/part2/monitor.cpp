#include "monitor.hpp"

void Monitor::lock(int id)
{
    arr.at(id).lock();
}
void Monitor::unlock(int id)
{
    arr.at(id).unlock();
}