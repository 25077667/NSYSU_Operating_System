#include "philosopher.hpp"

#include <unistd.h>

#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <string>

#include "semaphore.hpp"
#define PHILO_NUM 5

using namespace std;

static int random_func() {
    random_device rd;
    auto gen = mt19937_64(rd());
    uniform_int_distribution<int> dis(1, 101);
    return bind(dis, gen)();
}

Philosopher::Philosopher(char _id) : id(_id), state("Confused") {}

void Philosopher::think() {
    this->state = string("Confused");
    usleep(random_func());
    this->state = string("Hungry");
}

void Philosopher::eat() {
    this->state = string("Hungry");
    usleep(random_func());
    this->state = string("Confused");
}

void Philosopher::useChop(vector<unique_ptr<Semaphore>> &chopPool) {
    /* Show the waiting time to get chopsticks */
    auto start = chrono::steady_clock::now();

    chopPool.at(this->id).get()->wait();
    chopPool.at((this->id + 1) % PHILO_NUM).get()->wait();

    auto end = std::chrono::steady_clock::now();
    cout << (int)this->id << " takes "
         << chrono::duration<double>(end - start).count()
         << " second(s) for waiting chopsticks." << endl;

    eat();

    /* After eat, should put chopsticks back, don't make it your own */
    chopPool.at(this->id).get()->notify();
    chopPool.at((this->id + 1) % PHILO_NUM).get()->notify();
}

void lifeTime(Philosopher &p, vector<unique_ptr<Semaphore>> &chopPool) {
    // In 2/3 probability to go to eat
    bool isHungry = random_func() % 3;
    if (isHungry)
        p.useChop(chopPool);
    else
        p.think();
}