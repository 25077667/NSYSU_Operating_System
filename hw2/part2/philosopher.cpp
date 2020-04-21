#include "philosopher.hpp"
#include <unistd.h>
#include <functional>
#include <random>
#include <string>
#include "semaphore.hpp"

using namespace std;

static int random_func()
{
    random_device rd;
    auto gen = mt19937_64(rd());
    uniform_int_distribution<int> dis(0, 100);
    return bind(dis, gen)();
}

Philosopher::Philosopher(char _id) : id(_id), state("Confused") {}

void Philosopher::think()
{
    this->state = string("Confused");
    usleep(random_func());
    this->state = string("Hungry");
}

void Philosopher::eat()
{
    this->state = string("Hungry");
    usleep(random_func());
    this->state = string("Confused");
}
