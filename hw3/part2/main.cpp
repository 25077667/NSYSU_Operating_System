#include <unistd.h>

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
void monitor(vector<Philosopher> &philos, bool &state);

int main() {
    vector<Philosopher> philoPool;

    /**
     * vector cannot contain mutex, because "vector requires that the values are
     * movable".
     * Just like the answaer below here:
     * https://stackoverflow.com/questions/16465633/how-can-i-use-something-like-stdvectorstdmutex
     *
     * Could use std::unique_ptr<std::mutex> instead of std::mutex. unique_ptrs
     * are movable.
     */
    vector<unique_ptr<Semaphore>> chopPool;
    bool state = true;

    /* Init */
    for (int i = 0; i < PHILO_NUM; i++) {
        philoPool.push_back(Philosopher(i));
        chopPool.push_back(make_unique<Semaphore>());
    }
    cout << "There is " << PHILO_NUM << " Philosophers." << endl;
    auto _monitor = thread(&monitor, ref(philoPool), ref(state));

    for (int i = 0; i < 2 * PHILO_NUM; i++) {
        fflush(stdout);
        cout << "========== In " << i << "th testing ==========" << endl;
        sitDown(philoPool, chopPool);
    }
    state = false;
    _monitor.join();

    cout << "End of testing" << endl;
    return 0;
}
/**
 * This function name is a joke, either the philosopher eat or think,
 * they need to sit down first.
 * Using a "TESTING_DURATION" to control the testing period, that is they must
 * racing for several times.
 */
void sitDown(vector<Philosopher> &philos,
             vector<unique_ptr<Semaphore>> &chopstickPool) {
    // In some period keep testing
    for (auto start = chrono::steady_clock::now(),
              end = chrono::steady_clock::now();
         chrono::duration<double>(end - start).count() < TESTING_DURATION;
         end = chrono::steady_clock::now()) {
        vector<thread> t;
        for (auto &p : philos)
            t.push_back(thread(&lifeTime, ref(p), ref(chopstickPool)));
        for (auto &i : t)
            i.join();
    }
}

void monitor(vector<Philosopher> &philos, bool &state) {
    while (state) {
        for (auto i : philos)
            cout << ((i.state == "Hungry") ? "H " : "C ");
        cout << endl;
        usleep(20);
    }
}
