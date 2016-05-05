#include <iostream>

#include "RungeKuttaFehlberg.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
    RungeKuttaFehlberg rkf;

    rkf.test();

    return 0;
}
