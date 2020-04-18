#ifndef PHILOSOPHER_HPP
#define PHILOSOPHER_HPP
#include <string>
using namespace std;

class Philosopher
{
public:
    Philosopher(char);
    ~Philosopher();
    void think();
    void eat();
    char id;
    string state;
};
#endif