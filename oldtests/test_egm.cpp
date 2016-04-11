/*
 * Test for earth gravitation model.
 *
 */

#include <iostream>

#include "EGM96GravityModel.hpp"
#include "EGM08GravityModel.hpp"

#include "IERSConventions.hpp"

#include "SP3EphemerisStore.hpp"
#include "SatDataReader.hpp"

#include "CivilTime.hpp"

using namespace std;
using namespace gpstk;


int main(void)
{
    LoadIERSERPFile("../tables/finals2000A.all");
    LoadIERSLSFile("../tables/Leap_Second_History.dat");
    LoadJPLEphFile("../tables/JPLEPH2000");

    int n;
    cout << "Enter Sat ID: ";
    cin >> n;

    // SatID
    SatID satid(n,SatID::systemGPS);

    // time
    CivilTime ct(2015,1,1,12,0,0.0, TimeSystem::GPS);
    CommonTime gps( ct.convertToCommonTime() );
    CommonTime utc( GPS2UTC(gps) );
    cout << ct << endl;

    // sp3 file
    SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.rejectBadClocks(true);
    sp3Eph.setPosGapInterval(900+1);
    sp3Eph.setPosMaxInterval(9*900+1);

    try {
    
        sp3Eph.loadFile("../workplace/igs18253.sp3");
        sp3Eph.loadFile("../workplace/igs18254.sp3");
        sp3Eph.loadFile("../workplace/igs18255.sp3");
    }
    catch(...) {
    
        cerr << "sp3 file load error." << endl;
    }

    Vector<double> r_ecef = sp3Eph.getXvt(satid, gps).x.toVector();
    Vector<double> v_ecef = sp3Eph.getXvt(satid, gps).v.toVector();

    Matrix<double> c2t( C2TMatrix(utc) );
    Matrix<double> dc2t( dC2TMatrix(utc) );

    Vector<double> r_eci = transpose(c2t) * r_ecef;
    Vector<double> v_eci = transpose(dc2t) * r_ecef + transpose(c2t) * v_ecef;

    cout << fixed << setprecision(6);
    cout << "r_eci: " << r_eci << endl;
    cout << "v_eci: " << v_eci << endl;

    // initial conditions
    Vector<double> rv0(6,0.0);
    rv0[0] = r_eci(0); rv0[1] = r_eci(1); rv0[2] = r_eci(2);
    rv0[3] = v_eci(0); rv0[4] = v_eci(1); rv0[5] = v_eci(2);

    // earth body
    EarthBody eb;

    Vector<double> p0;
    p0.resize(3,0.0);

    // sat data
    SatDataReader satData("../tables/SATELLITE");

    // spacecraft
    Spacecraft sc;
    sc.setSatID(satid);
    sc.setEpoch(utc);
    sc.setBlock(satData.getBlock(satid,utc));
    sc.setMass(satData.getMass(satid,utc));

    sc.initStateVector(rv0, p0);

    cout << fixed << setprecision(15);

    EGM08GravityModel egm08(12,12);
    egm08.enableSolidTide(true);
    egm08.enableOceanTide(true);
    egm08.enablePoleTide(true);
    egm08.doCompute(utc, eb, sc);

    cout << "EGM08: " << egm08.getAccel() << endl;


    return 0;
}
