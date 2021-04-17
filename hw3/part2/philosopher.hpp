#ifndef __PHILOSOPHER_HPP__
#define __PHILOSOPHER_HPP__
#include <iostream>
#include "monitor.hpp"

class Philosopher
{
    void think();
    void eat();

    void take_chop(Monitor &m);
    void put_chop(Monitor &m);

    int id;
    char state;

public:
    Philosopher(int _id) : id(_id), state('t') {}
    ~Philosopher() = default;
    char get_state() { return state; }
    void do_job(Monitor &m)
    {
        std::cout << "hi" << this->id << std::endl;
        while (true) {
            think();
            take_chop(m);
            eat();
            put_chop(m);
        }
    };
};

#endif