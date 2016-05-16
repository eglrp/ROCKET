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
        confReader.open("../../ROCKET/oldtests/test.conf");
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
    Vector<double> r_sun(3,0.0), r_moon(3,0.0);
    r_sun(0) = rv_sun(0); r_sun(1) = rv_sun(1); r_sun(2) = rv_sun(2);
    r_moon(0) = rv_moon(0); r_moon(1) = rv_moon(1); r_moon(2) = rv_moon(2);

    cout << "r_sun: " << r_sun << endl;
    cout << "r_moon: " << r_moon << endl;

//    double jd( JulianDate(tt).jd );
//    double rv_sun1[6], rv_moon1[6];
//    SolarSystem ss;
//    ss.initializeWithBinaryFile(ephFile);
//    ss.computeState(jd, SolarSystem::Sun, SolarSystem::Earth, rv_sun1);
//    ss.computeState(jd, SolarSystem::Moon, SolarSystem::Earth, rv_moon1);
//    cout << "r_sun: "
//         << rv_sun1[0] << ' ' << rv_sun1[1] << ' ' << rv_sun1[2] << ' '
//         << rv_sun1[3] << ' ' << rv_sun1[4] << ' ' << rv_sun1[5] << endl;
//    cout << "r_moon: "
//         << rv_moon1[0] << ' ' << rv_moon1[1] << ' ' << rv_moon1[2] << ' '
//         << rv_moon1[3] << ' ' << rv_moon1[4] << ' ' << rv_moon1[5] << endl;

    // Position of Sun and Moon, from Analytical Formulas
    Vector<double> r_sun2( SunJ2kPosition(tt) );
    Vector<double> r_moon2( MoonJ2kPosition(tt) );
    cout << "r_sun: " << r_sun2 << endl;
    cout << "r_moon: " << r_moon2 << endl;


    Vector<double> err_sun( (r_sun2 - r_sun)/r_sun );
    Vector<double> err_moon( (r_moon2 - r_moon)/r_moon );

    cout << "err_sun: " << err_sun << endl;
    cout << "err_moon: " << err_moon << endl;

    return 0;
}
