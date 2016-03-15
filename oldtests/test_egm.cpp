/*
 * Test for earth gravitation model.
 *
 */

#include <iostream>

#include "EGM96GravityModel.hpp"
#include "EGM08GravityModel.hpp"

#include "IERSConventions.hpp"

#include "SatDataReader.hpp"

#include "CivilTime.hpp"

using namespace std;
using namespace gpstk;


int main(void)
{
    LoadIERSERPFile("finals.data");
    LoadIERSLSFile("Leap_Second_History.dat");
    LoadJPLEphFile("JPLEPH2000");

    // SatID
    SatID satid(1,SatID::systemGPS);

    // time
    CivilTime ct(2015,1,1,12,0,0.0, TimeSystem::GPS);
    CommonTime gps( ct.convertToCommonTime() );
    CommonTime utc( GPS2UTC(gps) );

    // initial conditions
    Vector<double> rv0(6,0.0);
    rv0[0] = 17104535.124;
    rv0[1] = -20054219.934;
    rv0[2] = 3879892.231;
    rv0[3] = 2000.369791;
    rv0[4] = 1091.671857;
    rv0[5] = -3118.194868;
/*
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
*/
    EarthBody rb;

    Vector<double> p0;
    p0.resize(3,0.0);

    // sat data
    SatDataReader satData("SATELLITE");

    // spacecraft
    Spacecraft sc;
    sc.setSatID(satid);
    sc.setEpoch(utc);
    sc.setBlock(satData.getBlock(satid,utc));
    sc.setMass(satData.getMass(satid,utc));

    sc.initStateVector(rv0, p0);

    cout << setprecision(15);

    EGM08GravityModel egm08(8,8);
    egm08.enableSolidTide(true);
    egm08.enableOceanTide(true);
    egm08.enablePoleTide(true);
    egm08.doCompute(utc, rb, sc);

    cout << "EGM08: " << egm08.getAccel() << endl;


    return 0;
}
