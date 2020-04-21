#include <iostream>
#include <thread>
#include <vector>

#include "philosopher.hpp"
#include "semaphore.hpp"
#define PHILO_NUM 5
#define TESTING_DURATION 0.001
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
        fflush(stdout);
        cout << "========== In " << i << "th testing ==========" << endl;
        sitDown(philoPool, chopPool);
    }

    cout << "End of testing" << endl;
    return 0;
}

void sitDown(vector<Philosopher> &philos,
             vector<unique_ptr<Semaphore>> &chopstickPool)
{
    // In some period keep testing
    for (auto start = chrono::steady_clock::now(),
              end = std::chrono::steady_clock::now();
         chrono::duration<double>(end - start).count() < TESTING_DURATION;
         end = std::chrono::steady_clock::now()) {
        vector<thread> t;
        for (auto &p : philos)
            t.push_back(thread(&lifeTime, ref(p), ref(chopstickPool)));
        for (auto &i : t)
            i.join();
    }
}
