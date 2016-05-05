/* Test for DE405 Planet Ephemeris. */

#include <iostream>

#include "IERSConventions.hpp"

#include "Epoch.hpp"

using namespace std;
using namespace gpstk;

int main()
{
    // Load DE405 Planet Ephemeris file
    try
    {
        LoadJPLEphFile("../tables/JPLEPH2000");
    }
    catch(...)
    {
        cerr << "JPL Ephemeris File load error." << endl;
    }

    // Load IERS Leap Second file
    try
    {
        LoadIERSLSFile("../tables/Leap_Second_History.dat");
    }
    catch(...)
    {
        cerr << "IERS Leap Second File load error." << endl;
    }

    // Time to compute
    CivilTime ct(2010,1,1,12,0,0.0, TimeSystem::UTC);
    CommonTime utc( ct.convertToCommonTime() );
    CommonTime tt( UTC2TT(utc) );

    cout << ct << endl;

    cout << fixed << setprecision(4);

    // Position, Velocity of Sun and Moon, from DE 405 Planet Ephemeris
    Vector<double> r_sun( J2kPosVel(tt, SolarSystem::Sun) );
    Vector<double> r_moon( J2kPosVel(tt, SolarSystem::Moon) );

    cout << "r_sun: " << r_sun << endl;
    cout << "r_moon: " << r_moon << endl;

    // Position of Sun and Moon, from Analytical Formulas
    Vector<double> r_sun2( SunJ2kPosition(tt) );
    Vector<double> r_moon2( MoonJ2kPosition(tt) );
    cout << "r_sun: " << r_sun2 << endl;
    cout << "r_moon: " << r_moon2 << endl;

    return 0;
}
