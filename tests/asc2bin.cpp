#include <iostream>

#include "SolarSystem.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
    if(argc < 3) return 0;

    SolarSystem ss;

    ss.readASCIIheader(argv[1]);

    for(int i=2; i<argc; i++)
    {
        ss.readASCIIdata(argv[i]);
    }

    ss.writeBinaryFile("de405.txt");

    return 0;
}
