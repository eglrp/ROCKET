/* Test for DE 405 Planet Ephemeris. */

#include <iostream>

#include "ConfDataReader.hpp"

#include "IERSConventions.hpp"

#include "Epoch.hpp"

using namespace std;
using namespace gpstk;

int main()
{

    // Configuration file
    ConfDataReader confReader;

    try
    {
        confReader.open("../../rocket/oldtests/testconf.txt");
    }
    catch(...)
    {
        cerr << "Configuration file open error." << endl;

        return 1;
    }


    // JPL Ephemeris file
    string ephFile = confReader.getValue("JPLEPHFile", "DEFAULT");
    try
    {
        LoadJPLEphFile(ephFile);
    }
    catch(...)
    {
        cerr << "JPL Ephemeris File load error." << endl;
    }


    // IERS LeapSecond file
    string lsFile  = confReader.getValue("IERSLSFile", "DEFAULT");
    try
    {
        LoadIERSLSFile(lsFile);
    }
    catch(...)
    {
        cerr << "IERS Leap Second File load error." << endl;
    }

    // Time
    CivilTime ct(2010,1,1,12,0,0.0, TimeSystem::UTC);
    CommonTime utc( ct.convertToCommonTime() );
    CommonTime tt( UTC2TT(utc) );

    cout << ct << endl;

    cout << fixed << setprecision(4);

    // Position, Velocity of Sun and Moon, from DE 405 Planet Ephemeris
    Vector<double> rv_sun( J2kPosVel(tt, SolarSystem::Sun) );
    Vector<double> rv_moon( J2kPosVel(tt, SolarSystem::Moon) );

    cout << "rv_sun: " << rv_sun << endl;
    cout << "rv_moon: " << rv_moon << endl;

    // Position of Sun and Moon, from Analytical Formulas
    Vector<double> r_sun2( SunJ2kPosition(tt) );
    Vector<double> r_moon2( MoonJ2kPosition(tt) );
    cout << "r_sun: " << r_sun2 << endl;
    cout << "r_moon: " << r_moon2 << endl;

    return 0;
}
