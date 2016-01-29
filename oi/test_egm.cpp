#include <iostream>

#include "EGM96GravityModel.hpp"
#include "EGM08GravityModel.hpp"
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

    // GPS PRN-1 BLOCK IIF
    Vector<double> rv0(6,0.0);
    rv0[0] =  17253546.070962253;
    rv0[1] = -19971157.156486035;
    rv0[2] =  3645801.3286026035;
    rv0[3] =  1973.2092921706201;
    rv0[4] =  1123.3113892016910;
    rv0[5] = -3124.1454542639446;

    Vector<double> r_eci(3,0.0), v_eci(3,0.0);
    r_eci[0] = rv0[0]; r_eci[1] = rv0[1]; r_eci[2] = rv0[2];
    v_eci[0] = rv0[3]; v_eci[1] = rv0[4]; v_eci[2] = rv0[5];

    cout << fixed << setprecision(3);
    cout << "r_eci: " << r_eci << endl
         << "v_eci: " << v_eci << endl;

    Matrix<double> c2t( C2TMatrix(utc) );
    Matrix<double> dc2t( dC2TMatrix(utc) );
    Vector<double> r_ecef( c2t*r_eci );
    Vector<double> v_ecef( c2t*v_eci + dc2t*r_eci );

    cout << fixed << setprecision(3);
    cout << "r_ecef: " << r_ecef << endl
         << "v_ecef: " << v_ecef << endl;

    EarthBody rb;

    Vector<double> p0;
    p0.resize(3,0.0);

    Spacecraft sc;
    sc.initStateVector(rv0, p0);
    sc.setDryMass(1555.3);
    sc.setBlockNum(7);

//    EGM96GravityModel egm96(12,12);
//    egm96.enableSolidTide(true);
//    egm96.setOceanTideFile("OT_CSRC.TID");
//    egm96.enableOceanTide(true);
//    egm96.enablePoleTide(true);
//    egm96.doCompute(utc, rb, sc);


    EGM08GravityModel egm08(12,12);
//    egm08.enableSolidTide(true);
//    egm08.setOceanTideFile("OT_CSRC.TID");
//    egm08.enableOceanTide(true);
//    egm08.enablePoleTide(true);
    egm08.doCompute(utc, rb, sc);

    return 0;
}
