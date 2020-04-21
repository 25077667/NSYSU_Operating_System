#include <iostream>
#include <thread>
#include <vector>

#include "philosopher.hpp"
#include "semaphore.hpp"
#define PHILO_NUM 5
#define START_EAT 1
#define STOP_EAT 0
using namespace std;

void sitDown(vector<Philosopher> &philos,
             vector<unique_ptr<Semaphore>> &chopstickPool);

int main()
{
    vector<Philosopher> philoPool;
    vector<Semaphore> chopstickPool;
    vector<unique_ptr<Semaphore>> chopPool;

    /* Init */
    for (int i = 0; i < PHILO_NUM; i++) {
        philoPool.push_back(Philosopher(i));
        chopPool.push_back(make_unique<Semaphore>());
        // chopstickPool.push_back(Semaphore(1));
    }
    cout << "There is " << PHILO_NUM << " Philosophers." << endl;

    for (int i = 0; i < 2 * PHILO_NUM; i++) {
        sitDown(philoPool, chopPool);
    }

    cout << "End of testing" << endl;
    return 0;
}

void sitDown(vector<Philosopher> &philos,
             vector<unique_ptr<Semaphore>> &chopstickPool)
{
    vector<thread> t;
    for (auto &p : philos)
        t.push_back(thread(&lifeTime, ref(p), ref(chopstickPool)));
    for (auto &i : t)
        i.join();
}
