/*
 * Test for the functions in IERSConventions.hpp/cpp, mainly the transformation between
 * ITRS and ICRS.
 *
 * Test time: 2015/01/01,12h0m0.0s, UTC
 *
 * Compared with result from
 *   http://hpiers.obspm.fr/eop-pc/index.php?index=matrice&lang=en#description
 *
 *    0.184438630509  0.982842935794  0.001468038215
 *   -0.982843980602  0.184438898953 -0.000048456351
 *   -0.000318388334 -0.001433915300  0.999998921257
 *
 * When using the EOP data in final2000A.all, the difference is
 *   -3.9e-11   0.8e-11  -6.2e-10
 *   -0.7e-11  -4.0e-11   5.7e-10
 *    6.8e-10   5.0e-10   0.1e-11
 *
 * When using the EOP data in eopc04_08_IAU2000.62-now, the difference is
 *
 *   -6.2e-11   1.2e-11  -2.8e-10
 *   -1.1e-11   6.2e-11   2.3e-10
 *    2.8e-10   2.3e-10   0.1e-11
 */

#include <iostream>
#include "IERSConventions.hpp"
#include "SP3EphemerisStore.hpp"

#include "SunPosition.hpp"
#include "MoonPosition.hpp"
#include "Epoch.hpp"

#include "GeodeticFrames.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
    // load IERS ERP file
    try
    {
        LoadIERSERPFile("../../rocket/tables/finals2000A.all");
    }
    catch(...)
    {
        cerr << "IERS Earth Orientation Parameters File Load Error." << endl;

        return 1;
    }
    
    // load IERS LeapSecond file
    try
    {
        LoadIERSLSFile("../../rocket/tables/Leap_Second_History.dat");
    }
    catch(...)
    {
        cerr << "IERS Leap Second File Load Error." << endl;

        return 1;
    }

    // load JPL Ephemeris file
    try
    {
        LoadJPLEphFile("../../rocket/tables/JPLEPH2000");
    }
    catch(...)
    {
        cerr << "JPL Ephemeris File Load Error." << endl;

        return 1;
    }


    SP3EphemerisStore sp3Eph;
    sp3Eph.rejectBadPositions(true);
    sp3Eph.rejectBadPositions(true);
    sp3Eph.setPosGapInterval(900+1);
    sp3Eph.setPosMaxInterval(9*900+1);

    // load IGS SP3 file
    try
    {
        sp3Eph.loadFile("../../rocket/workplace/igs18253.sp3");
        sp3Eph.loadFile("../../rocket/workplace/igs18254.sp3");
        sp3Eph.loadFile("../../rocket/workplace/igs18255.sp3");
    }
    catch(...)
    {
        cerr << "SP3 File Load Error." << endl;
    }

    // epoch
    CivilTime ct(2015,1,1,12,0,0.0, TimeSystem::UTC);
    CommonTime UTC( ct.convertToCommonTime() );
    CommonTime GPS( UTC2GPS(UTC) );

    cout << fixed << setprecision(4);
    cout << ct << endl;

    // transformation matrix
    Matrix<double> C2T( C2TMatrix(UTC) );
    cout << fixed << setprecision(12);
    cout << "C2T: " << endl << C2T << endl;
    // transformation matrix time dot
    Matrix<double> dC2T( dC2TMatrix(UTC) );
    cout << fixed << setprecision(12);
    cout << "dC2T: " << endl << dC2T << endl;

    // satid
    SatID sat(1, SatID::systemGPS);

    // position and velocity in ECEF
    Vector<double> r_ecef(3,0.0), v_ecef(3,0.0);
    try
    {
        r_ecef = sp3Eph.getXvt(sat,GPS).x.toVector();
        v_ecef = sp3Eph.getXvt(sat,GPS).v.toVector();
    }
    catch(...)
    {
        cerr << "Get ECEF Position and Velocity from SP3 File Error." << endl;
    }

    // position and velocity in ECI
    Vector<double> r_eci( transpose(C2T)*r_ecef );
    Vector<double> v_eci( transpose(C2T)*v_ecef + transpose(dC2T)*r_ecef);

    // output
//    cout << fixed << setprecision(6);
//    cout << "r_ecef: " << endl;
//    cout << setw(12) << r_ecef << endl;
//    cout << "v_ecef: " << endl;
//    cout << setw(12) << v_ecef << endl;
//    cout << "r_eci: " << endl;
//    cout << setw(12) << r_eci << endl;
//    cout << "v_eci: " << endl;
//    cout << setw(12) << v_eci << endl;

    return 0;
}
