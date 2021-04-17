#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "monitor.hpp"
#include "philosopher.hpp"
using namespace std;

int main()
{
    Monitor mon;
    std::vector<Philosopher> v;
    for (int i = 0; i < NUM; i++)
        v.push_back(Philosopher(i));

    for (auto &i : v) {
        std::thread t(&Philosopher::do_job, ref(i), ref(mon));
        t.detach();
    }

    while (true) {
        for (auto &i : v)
            std::cout << i.get_state() << " ";
        std::cout << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}