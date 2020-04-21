#ifndef PHILOSOPHER_HPP
#define PHILOSOPHER_HPP
#include <string>
using namespace std;

class Philosopher
{
public:
    Philosopher(char);
    void think();
    void eat();
    char id;
    string state;
};
#endif