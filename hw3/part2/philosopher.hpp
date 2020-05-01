#ifndef PHILOSOPHER_HPP
#define PHILOSOPHER_HPP
#include <string>
#include <vector>
#define TESTING_DURATION_SEC 0.001

#include "semaphore.hpp"
using namespace std;

class Philosopher {
   public:
    Philosopher(char);

    /**
     * The life of a philosopher consists of alternate periods of eating and
     * thinking.
     */
    void think();
    void eat();

    /*
     * Before the philosopher eat, he/she needs to "pick up" the chopstick.
     * After dinning, he/she needs to "put back" the chopstick.
     */
    void useChop(vector<unique_ptr<Semaphore>> &);

    /* Use char to store id, reduce the memory cost */
    char id;

    /*
     * String is easier to set the "state"
     * Actually, we can use bool to store the "state"
     * But, I just want to use string.
     */
    string state;
};

/**
 * Sometimes philosopher goes eat, sometimes geos think.
 * Use random function to adjust the probability of eating.
 */
void lifeTime(Philosopher &p, vector<unique_ptr<Semaphore>> &chopPool);
#endif