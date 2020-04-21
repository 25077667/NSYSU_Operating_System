#include <iostream>
#include <thread>
#include <vector>
#include "philosopher.hpp"
#include "semaphore.hpp"
#define PHILO_NUM 5
using namespace std;

void take_fork(int id, vector<Semaphore>& chopstickPool);
void put_fork(int id, vector<Semaphore>& chopstickPool);

int main()
{
    vector<Philosopher> philoPool;
    vector<Semaphore> chopstickPool;
    for(int i = 0; i < PHILO_NUM; i++)
        philoPool.push_back(Philosopher(i));
    
    return 0;
}
