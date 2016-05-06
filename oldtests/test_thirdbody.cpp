#include <iostream>

#include "SunForce.hpp"
#include "MoonForce.hpp"
#include "IERSConventions.hpp"
#include "Epoch.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
    LoadIERSERPFile("finals.data");
    LoadIERSLSFile("Leap_Second_History.dat");
    LoadJPLEphFile("JPLEPH2000");

    CivilTime cv0(2015,1,1,12,1,15.0, TimeSystem::GPS);
    CommonTime gps0( cv0.convertToCommonTime() );
    CommonTime utc0( GPS2UTC(gps0) );

    // G01, BLOCK IIF
    Vector<double> rv0(6,0.0);
    rv0(0) =  17253546.070962253;
    rv0(1) = -19971157.156486035;
    rv0(2) =  3645801.3286026035;
    rv0(3) =  1973.2092921706201;
    rv0(4) =  1123.3113892016910;
    rv0(5) = -3124.1454542639446;

    // force mdoel parameters
    Vector<double> p0;
    
    // Earth reference body
    EarthBody rb;

    // Spacecraft settings
    Spacecraft sc;
    sc.initStateVector(rv0, p0);
    sc.setDryMass(1555.3);
    sc.setBlockNum(7);

    // Sun gravitation
    SunForce sun;
    sun.doCompute(utc0, rb, sc);

    Vector<double> Gsun( sun.getAccel() );
    double Tsun( norm(Gsun) );

    cout << setprecision(15);

    cout << "Sun gravity: " << endl
         << setw(16) << Gsun << ' '
         << setw(16) << Tsun << endl;

    // Moon gravitation
    MoonForce moon;
    moon.doCompute(utc0, rb, sc);

    Vector<double> Gmoon( moon.getAccel() );
    double Tmoon( norm(Gmoon) );

    cout << "Moon gravity: " << endl
         << setw(16) << Gmoon << ' '
         << setw(16) << Tmoon << endl;

    return 0;
}
