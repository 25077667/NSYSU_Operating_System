#include "colorprint.hpp"
#define RESET "\033[0m"
#define BLACK "\033[1m\033[30m"   /*  Black */
#define RED "\033[1m\033[31m"     /*  Red */
#define GREEN "\033[1m\033[32m"   /*  Green */
#define YELLOW "\033[1m\033[33m"  /*  Yellow */
#define BLUE "\033[1m\033[34m"    /*  Blue */
#define MAGENTA "\033[1m\033[35m" /*  Magenta */
#define CYAN "\033[1m\033[36m"    /*  Cyan */
#define WHITE "\033[1m\033[37m"   /*  White */

#include <iostream>
#include <string>
#include <vector>
using namespace std;

void print_color(string s, bool doRainbow)
{
    vector<string> colorSet;
    if (doRainbow)
        colorSet = {RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE};
    else
        colorSet = {RESET};
    for (unsigned int i = 0; i < s.length(); i++) {
        cout << colorSet.at(i % colorSet.size()) << s.at(i) << RESET;
    }
    cout << endl;
}
