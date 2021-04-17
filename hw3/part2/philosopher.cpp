#include "philosopher.hpp"
#include <unistd.h>
#include <chrono>
#include <thread>

void Philosopher::think()
{
    this->state = 't';
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));
}

void Philosopher::eat()
{
    this->state = 'e';
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));
}

void Philosopher::take_chop(Monitor &m)
{
    this->state = 'w';
    m.lock(this->id);
    m.lock((this->id + 1) % NUM);
}

void Philosopher::put_chop(Monitor &m)
{
    m.unlock(this->id);
    m.unlock((this->id + 1) % NUM);
}
