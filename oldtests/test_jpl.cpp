#include <iostream>

#include "SolarSystem.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
    SolarSystem ss;
    ss.readASCIIheader("header.405");
//    ss.readASCIIdata("ascp1980.405");
    ss.readASCIIdata("ascp2000.405");
//    ss.readASCIIdata("ascp2020.405");
    ss.writeBinaryFile("JPLEPH1980_2000");

    return 0;
}
