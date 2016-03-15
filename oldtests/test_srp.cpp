#include "SolarPressure.hpp"

#include "ROCKPressure.hpp"
#include "CODEPressure.hpp"

#include "IERSConventions.hpp"

#include "CivilTime.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
    LoadIERSERPFile("finals.data");
    LoadIERSLSFile("Leap_Second_History.dat");
    LoadJPLEphFile("JPLEPH2000");

    CivilTime ct(2015,1,1,12,1,15.0, TimeSystem::GPS);
    CommonTime gps( ct.convertToCommonTime() );
    CommonTime utc( GPS2UTC(gps) );

    Vector<double> rv(6,0.0);
    rv[0] =  17253.546071e3;
    rv[1] = -19971.1571565e3;
    rv[2] =   3645.8013286e3;
    rv[3] =  1.9732093e3;
    rv[4] =  1.1233114e3;
    rv[5] = -3.1241455e3;

    Vector<double> p(9,0.0);
    p[0] = 0.999989506;
    p[1] = -0.000367154;
    p[2] = -0.003625165;
    p[3] = 0.015272206;
    p[4] = 0.000216184;
    p[5] = -0.000701573;
    p[6] = 0.000985358;
    p[7] = 0.009091403;
    p[8] = -0.001752761;

    EarthBody rb;

    Spacecraft sc;
    sc.initStateVector(rv, p);
    sc.setDryMass(1555.3);      // in kg
    sc.setBlockNum(7);          // BLOCK-IIF

    CODEPressure code;
    code.doCompute(utc, rb, sc);

    Vector<double> f_srp(3,0.0);
    f_srp = code.getAccel();

    cout << fixed << setprecision(15);
    cout << "CODE SRP: " << endl
         << f_srp << endl;

    return 0;
}
