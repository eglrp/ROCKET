/* Test for DE 405 Planet Ephemeris. */

#include <iostream>

#include "ConfDataReader.hpp"

#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"

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


    // Ephemeris Data
    SolarSystem solSys;

    string ephFile = confReader.getValue("JPLEPHFile", "DEFAULT");
    try
    {
        solSys.initializeWithBinaryFile(ephFile);
    }
    catch(...)
    {
        cerr << "JPL Ephemeris File load error." << endl;
    }


    // LeapSecond Data
    LeapSecStore leapSecStore;

    string lsFile  = confReader.getValue("IERSLSFile", "DEFAULT");
    try
    {
        leapSecStore.loadFile(lsFile);
    }
    catch(...)
    {
        cerr << "IERS Leap Second File load error." << endl;
    }

    // Reference System
    ReferenceSystem refSys;
    refSys.setLeapSecStore(leapSecStore);

    // Time
    CivilTime ct(2010,1,1,12,0,0.0, TimeSystem::UTC);
    CommonTime utc( ct.convertToCommonTime() );
    double tt( JulianDate(refSys.UTC2TT(utc)).jd );

    cout << ct << endl;

    cout << fixed << setprecision(4);

    // Position, Velocity of Sun and Moon, from DE 405 Planet Ephemeris
    double rv_sun[6] = {0.0};
    solSys.computeState(tt, SolarSystem::Sun, SolarSystem::Earth, rv_sun);
    Vector<double> r_sun(3,0.0);
    r_sun(0) = rv_sun[0];
    r_sun(1) = rv_sun[1];
    r_sun(2) = rv_sun[2];

    double rv_moon[6] = {0.0};
    solSys.computeState(tt, SolarSystem::Moon, SolarSystem::Earth, rv_moon);
    Vector<double> r_moon(3,0.0);
    r_moon(0) = rv_moon[0];
    r_moon(1) = rv_moon[1];
    r_moon(2) = rv_moon[2];

    cout << "r_sun: " << r_sun << endl;
    cout << "r_moon: " << r_moon << endl;

    return 0;
}
