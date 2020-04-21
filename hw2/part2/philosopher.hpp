#ifndef PHILOSOPHER_HPP
#define PHILOSOPHER_HPP
#include <string>
#include <vector>

#include "semaphore.hpp"
using namespace std;

class Philosopher
{
public:
    Philosopher(char);
    void think();
    void eat();
    void useChop(vector<unique_ptr<Semaphore>> &);
    char id;
    string state;
};

void lifeTime(Philosopher &p, vector<unique_ptr<Semaphore>> &chopPool);
#endif