#include <iostream>

#include "SolarSystem.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
    SolarSystem ss;
    ss.readASCIIheader("../../rocket/tables/header.405");
    ss.readASCIIdata("../../rocket/tables/ascp1980.405");
    ss.readASCIIdata("../../rocket/tables/ascp2000.405");
    ss.readASCIIdata("../../rocket/tables/ascp2020.405");
    ss.readASCIIdata("../../rocket/tables/ascp2040.405");
    ss.writeBinaryFile("../../rocket/tables/1980_2040.405");
    return 0;
}
