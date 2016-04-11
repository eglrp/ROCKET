#include <iostream>

#include "IERSConventions.hpp"

#include "Epoch.hpp"

using namespace std;
using namespace gpstk;

int main()
{
    try
    {
        LoadJPLEphFile("../tables/JPLEPH2000");
    }
    catch(...)
    {
        cerr << "JPL Ephemeris File load error." << endl;
    }

    try
    {
        LoadIERSLSFile("../tables/Leap_Second_History.dat");
    }
    catch(...)
    {
        cerr << "IERS Leap Second File load error." << endl;
    }

    CivilTime ct(2010,1,1,12,0,0.0, TimeSystem::UTC);
    CommonTime utc( ct.convertToCommonTime() );
    CommonTime tt( UTC2TT(utc) );

    cout << ct << endl;

    cout << fixed << setprecision(4);

    Vector<double> r_sun( J2kPosVel(tt, SolarSystem::Sun) );
    Vector<double> r_moon( J2kPosVel(tt, SolarSystem::Moon) );

    cout << "r_sun: " << r_sun << endl;
    cout << "r_moon: " << r_moon << endl;

    Vector<double> r_sun2( SunJ2kPosition(tt) );
    Vector<double> r_moon2( MoonJ2kPosition(tt) );
    cout << "r_sun: " << r_sun2 << endl;
    cout << "r_moon: " << r_moon2 << endl;

    return 0;
}
